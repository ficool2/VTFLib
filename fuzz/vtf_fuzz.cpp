/*
 * VTFFuzzer
 * Copyright (C) 2026 ficool2
 
 * usage:
 *   vtf_fuzz [-n iterations] [-s seed] [-o outdir] [corpus-dir ...]
 *   vtf_fuzz -f file.vtf          replay a single input
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <random>

#include <dirent.h>
#include <sys/stat.h>

#include <ctime>

#include "VTFFile.h"
#include "VTFWrapper.h"

using namespace VTFLib;

typedef std::vector<uint8_t> Buffer;

static std::mt19937 g_rng;
static std::string g_outDir = "fuzz-out";

static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(g_rng() % n) : 0; }

static void ExerciseImage(CVTFFile &File)
{
	vlUInt uiFrames = File.GetFrameCount();
	vlUInt uiFaces = File.GetFaceCount();
	vlUInt uiSlices = File.GetDepth();
	vlUInt uiMips = File.GetMipmapCount();

	File.GetFlags();
	File.GetStartFrame();
	vlSingle x, y, z;
	File.GetReflectivity(x, y, z);
	File.GetBumpmapScale();
	File.GetFormat();
	File.GetHasImage();
	File.GetHasThumbnail();
	File.GetSupportsResources();
	File.GetSupportsAuxCompression();

	// resource dict
	for (vlUInt i = 0; i < File.GetResourceCount(); i++)
	{
		vlUInt uiType = File.GetResourceType(i);
		vlUInt uiSize = 0;
		File.GetResourceData(uiType, uiSize);
	}

	// thumbnail
	if (File.GetHasThumbnail())
	{
		vlUInt uiWidth = File.GetThumbnailWidth();
		vlUInt uiHeight = File.GetThumbnailHeight();
		VTFImageFormat Format = File.GetThumbnailFormat();
		vlByte *lpData = File.GetThumbnailData();
		if (lpData != 0 && uiWidth && uiHeight &&
		   (vlUInt)uiWidth * (vlUInt)uiHeight < 4096 * 4096)
		{
			Buffer Dest((size_t)uiWidth * uiHeight * 4);
			CVTFFile::ConvertToRGBA8888(lpData, &Dest[0], uiWidth, uiHeight, Format);
		}
	}

	// cap so this doesn't take ages
	if ((vlUInt64)uiFrames * uiFaces * uiSlices * uiMips > 4096)
		return;

	for (vlUInt uiFrame = 0; uiFrame < uiFrames; uiFrame++)
	{
		for (vlUInt uiFace = 0; uiFace < uiFaces; uiFace++)
		{
			for (vlUInt uiSlice = 0; uiSlice < uiSlices; uiSlice++)
			{
				for (vlUInt uiMip = 0; uiMip < uiMips; uiMip++)
				{
					vlUInt uiWidth, uiHeight, uiDepth;
					CVTFFile::ComputeMipmapDimensions(File.GetWidth(), File.GetHeight(), File.GetDepth(), uiMip, uiWidth, uiHeight, uiDepth);

					vlByte *lpData = File.GetData(uiFrame, uiFace, uiSlice, uiMip);
					if (lpData == 0 || uiWidth == 0 || uiHeight == 0)
						continue;
					if ((vlUInt64)uiWidth * uiHeight > 2048 * 2048)
						continue;

					Buffer Dest((size_t)uiWidth * uiHeight * 4);
					CVTFFile::ConvertToRGBA8888(lpData, &Dest[0], uiWidth, uiHeight, File.GetFormat());
				}
			}
		}
	}
}

static void RunOne(const uint8_t *lpData, size_t uiSize)
{
	{
		CVTFFile File;
		File.Load(lpData, (vlUInt)uiSize, vlTrue);
	}

	CVTFFile File;
	if (!File.Load(lpData, (vlUInt)uiSize, vlFalse))
		return;

	ExerciseImage(File);

	vlUInt uiSaveSize = File.GetSize();
	if (uiSaveSize && uiSaveSize < 64 * 1024 * 1024)
	{
		Buffer Out(uiSaveSize);
		vlUInt uiWritten = 0;
		File.Save(&Out[0], uiSaveSize, uiWritten);
	}
}

// Corpus
static bool ReadFile(const std::string &Path, Buffer &Out)
{
	FILE *pFile = fopen(Path.c_str(), "rb");
	if (!pFile)
		return false;
	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	Out.resize(lSize > 0 ? (size_t)lSize : 0);
	if (!Out.empty() && fread(&Out[0], 1, Out.size(), pFile) != Out.size())
	{
		fclose(pFile);
		return false;
	}
	fclose(pFile);
	return true;
}

static void WriteFile(const std::string &Path, const Buffer &Data)
{
	FILE *pFile = fopen(Path.c_str(), "wb");
	if (!pFile)
		return;
	if (!Data.empty())
		fwrite(&Data[0], 1, Data.size(), pFile);
	fclose(pFile);
}

static void LoadCorpusDir(const std::string &Dir, std::vector<Buffer> &Corpus)
{
	DIR *pDir = opendir(Dir.c_str());
	if (!pDir)
	{
		fprintf(stderr, "vtf_fuzz: cannot open corpus directory '%s'\n", Dir.c_str());
		return;
	}
	struct dirent *pEntry;
	while((pEntry = readdir(pDir)) != 0)
	{
		std::string Name = pEntry->d_name;
		if (Name == "." || Name == "..")
			continue;
		std::string Path = Dir + "/" + Name;
		struct stat Stat;
		if (stat(Path.c_str(), &Stat) != 0 || !S_ISREG(Stat.st_mode))
			continue;
		Buffer Data;
		if (ReadFile(Path, Data) && !Data.empty())
			Corpus.push_back(Data);
	}
	closedir(pDir);
}

static void GenerateCorpus(std::vector<Buffer> &Corpus)
{
	static const VTFImageFormat Formats[] =
	{
		IMAGE_FORMAT_RGBA8888,
		IMAGE_FORMAT_DXT1,
		IMAGE_FORMAT_DXT5,
		IMAGE_FORMAT_BGR888,
		IMAGE_FORMAT_I8,
		IMAGE_FORMAT_RGBA16161616F
		// fixme bc7 error
	};
	static const vlUInt Minors[] = { 0, 2, 3, 4, 5 };

	const vlUInt uiWidth = 16, uiHeight = 16;
	Buffer Source((size_t)uiWidth * uiHeight * 4);
	for (size_t i = 0; i < Source.size(); i++)
		Source[i] = (uint8_t)(i * 7 + (i >> 3));

	for (size_t f = 0; f < sizeof(Formats) / sizeof(Formats[0]); f++)
	{
		for (size_t v = 0; v < sizeof(Minors) / sizeof(Minors[0]); v++)
		{
			SVTFCreateOptions Options;
			vlImageCreateDefaultCreateStructure(&Options);
			Options.ImageFormat = Formats[f];
			Options.uiVersion[0] = 7;
			Options.uiVersion[1] = Minors[v];
			Options.bMipmaps = vlTrue;
			Options.bThumbnail = vlTrue;
			Options.bReflectivity = vlTrue;
			Options.bResize = vlFalse;

			CVTFFile File;
			if (!File.Create(uiWidth, uiHeight, &Source[0], Options))
				continue;

			vlUInt uiSize = File.GetSize();
			Buffer Out(uiSize);
			vlUInt uiWritten = 0;
			if (File.Save(&Out[0], uiSize, uiWritten) && uiWritten)
			{
				Out.resize(uiWritten);
				Corpus.push_back(Out);
			}
		}
	}

	// animated cubemap
	{
		SVTFCreateOptions Options;
		vlImageCreateDefaultCreateStructure(&Options);
		Options.ImageFormat = IMAGE_FORMAT_DXT5;
		Options.uiVersion[0] = 7;
		Options.uiVersion[1] = 5;
		Options.uiFlags |= TEXTUREFLAGS_ENVMAP;
		Options.bMipmaps = vlTrue;
		Options.bThumbnail = vlTrue;

		vlByte *lpFaces[12];
		for (vlUInt i = 0; i < 12; i++)
			lpFaces[i] = &Source[0];

		CVTFFile File;
		if (File.Create(uiWidth, uiHeight, 2, 6, 1, lpFaces, Options))
		{
			vlUInt uiSize = File.GetSize();
			Buffer Out(uiSize);
			vlUInt uiWritten = 0;
			if (File.Save(&Out[0], uiSize, uiWritten) && uiWritten)
			{
				Out.resize(uiWritten);
				Corpus.push_back(Out);
			}
		}
	}

	// minimal header
	{
		Buffer Out(sizeof(SVTFHeader), 0);
		memcpy(&Out[0], "VTF\0", 4);
		Out[4] = 7;
		Out[8] = 2;
		Corpus.push_back(Out);
	}
}

// Mutation
static const uint32_t g_Interesting32[] =
{
	0, 1, 2, 3, 4, 8, 16, 0x7F, 0x80, 0xFF, 0x100, 0x1000, 0xFFFF, 0x10000,
	0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFF0000
};

static void Mutate(Buffer &Data, const std::vector<Buffer> &Corpus)
{
	uint32_t uiRounds = 1 + rnd(8);
	for (uint32_t r = 0; r < uiRounds; r++)
	{
		if (Data.empty())
			Data.push_back(0);

		switch (rnd(9))
		{
			case 0: // flip a bit
				Data[rnd((uint32_t)Data.size())] ^= (uint8_t)(1 << rnd(8));
				break;
			case 1: // set a random byte
				Data[rnd((uint32_t)Data.size())] = (uint8_t)rnd(256);
				break;
			case 2: // splat an interesting 32-bit value
			{
				uint32_t uiValue = g_Interesting32[rnd(sizeof(g_Interesting32) / sizeof(g_Interesting32[0]))];
				if (Data.size() >= 4)
					memcpy(&Data[rnd((uint32_t)Data.size() - 3)], &uiValue, 4);
				break;
			}
			case 3: // add/subtract a small delta
			{
				size_t uiOffset = rnd((uint32_t)Data.size());
				Data[uiOffset] = (uint8_t)(Data[uiOffset] + (int)rnd(35) - 17);
				break;
			}
			case 4: // overwrite a run
			{
				size_t uiOffset = rnd((uint32_t)Data.size());
				size_t uiLength = 1 + rnd(32);
				uint8_t uiFill = (uint8_t)rnd(256);
				for (size_t i = uiOffset; i < Data.size() && i < uiOffset + uiLength; i++)
					Data[i] = uiFill;
				break;
			}
			case 5: // truncate
				if (Data.size() > 1)
					Data.resize(1 + rnd((uint32_t)Data.size() - 1));
				break;
			case 6: // grow with junk
			{
				size_t uiLength = 1 + rnd(256);
				for (size_t i = 0; i < uiLength && Data.size() < (1 << 20); i++)
					Data.push_back((uint8_t)rnd(256));
				break;
			}
			case 7: // erase a chunk
			{
				if (Data.size() > 2)
				{
					size_t uiOffset = rnd((uint32_t)Data.size() - 1);
					size_t uiLength = 1 + rnd((uint32_t)(Data.size() - uiOffset - 1));
					Data.erase(Data.begin() + uiOffset, Data.begin() + uiOffset + uiLength);
				}
				break;
			}
			case 8: // splice in a chunk from another corpus entry
			{
				if (!Corpus.empty())
				{
					const Buffer &Other = Corpus[rnd((uint32_t)Corpus.size())];
					if (!Other.empty())
					{
						size_t uiSrc = rnd((uint32_t)Other.size());
						size_t uiLength = 1 + rnd((uint32_t)(Other.size() - uiSrc));
						size_t uiDst = rnd((uint32_t)Data.size());
						for (size_t i = 0; i < uiLength && uiDst + i < Data.size(); i++)
							Data[uiDst + i] = Other[uiSrc + i];
					}
				}
				break;
			}
		}
	}

	// always keep the magic!!
	if (Data.size() >= 4 && rnd(4) != 0)
		memcpy(&Data[0], "VTF\0", 4);
}

int main(int argc, char **argv)
{
	vlUInt64 uiIterations = 100000;
	uint32_t uiSeed = 0;
	bool bHaveSeed = false;
	const char *pReplay = 0;
	std::vector<std::string> CorpusDirs;

	for (int i = 1; i < argc; i++)
	{
		std::string Arg = argv[i];
		if (Arg == "-n" && i + 1 < argc)
			uiIterations = strtoull(argv[++i], 0, 10);
		else if (Arg == "-s" && i + 1 < argc)
		{
			uiSeed = (uint32_t)strtoul(argv[++i], 0, 10);
			bHaveSeed = true;
		}
		else if (Arg == "-o" && i + 1 < argc)
			g_outDir = argv[++i];
		else if (Arg == "-f" && i + 1 < argc)
			pReplay = argv[++i];
		else if (Arg == "-h" || Arg == "--help")
		{
			printf("usage: %s [-n iterations] [-s seed] [-o outdir] [corpus-dir ...]\n"
			       "       %s -f file.vtf\n", argv[0], argv[0]);
			return 0;
		}
		else
			CorpusDirs.push_back(Arg);
	}

	if (pReplay)
	{
		Buffer Data;
		if (!ReadFile(pReplay, Data))
		{
			fprintf(stderr, "vtf_fuzz: cannot read '%s'\n", pReplay);
			return 1;
		}
		printf("vtf_fuzz: replaying %s (%zu bytes)\n", pReplay, Data.size());
		RunOne(Data.empty() ? (const uint8_t *)"" : &Data[0], Data.size());
		printf("vtf_fuzz: clean\n");
		return 0;
	}

	if (!bHaveSeed)
		uiSeed = (uint32_t)time(0);
	g_rng.seed(uiSeed);

	std::vector<Buffer> Corpus;
	for (size_t i = 0; i < CorpusDirs.size(); i++)
		LoadCorpusDir(CorpusDirs[i], Corpus);
	if (Corpus.empty())
		GenerateCorpus(Corpus);
	if (Corpus.empty())
	{
		fprintf(stderr, "vtf_fuzz: empty corpus\n");
		return 1;
	}

	mkdir(g_outDir.c_str(), 0755);
	std::string LastPath = g_outDir + "/last_input.vtf";

	printf("vtf_fuzz: seed %u, %zu corpus entries, %llu iterations\n",
	       uiSeed, Corpus.size(), (unsigned long long)uiIterations);
	fflush(stdout);

	for (vlUInt64 i = 0; i < uiIterations; i++)
	{
		Buffer Data = Corpus[rnd((uint32_t)Corpus.size())];
		Mutate(Data, Corpus);

		WriteFile(LastPath, Data);

		RunOne(Data.empty() ? (const uint8_t *)"" : &Data[0], Data.size());

		if ((i % 5000) == 0)
		{
			printf("  %llu / %llu\n", (unsigned long long)i, (unsigned long long)uiIterations);
			fflush(stdout);
		}
	}

	printf("vtf_fuzz: done, no crashes\n");
	return 0;
}
