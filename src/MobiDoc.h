/* Copyright 2011-2012 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */

#ifndef MobiDoc_h
#define MobiDoc_h

#include "BaseUtil.h"
#include "Vec.h"

class HuffDicDecompressor;

// http://en.wikipedia.org/wiki/PDB_(Palm_OS)
#define kDBNameLength    32
#define kPdbHeaderLen    78

#pragma pack(push)
#pragma pack(1)
struct PdbHeader
{
     /* 31 chars + 1 null terminator */
    char        name[kDBNameLength];
    uint16      attributes;
    uint16      version;
    uint32      createTime;
    uint32      modifyTime;
    uint32      backupTime;
    uint32      modificationNumber;
    uint32      appInfoID;
    uint32      sortInfoID;
    char        type[4];
    char        creator[4];
    uint32      idSeed;
    uint32      nextRecordList;
    uint16      numRecords;
};
#pragma pack(pop)

STATIC_ASSERT(kPdbHeaderLen == sizeof(PdbHeader), validPdbHeader);

#define kPdbRecordHeaderLen 8

#pragma pack(push)
#pragma pack(1)
struct PdbRecordHeader {
    uint32   offset;
    uint8    deleted   : 1;
    uint8    dirty     : 1;
    uint8    busy      : 1;
    uint8    secret    : 1;
    uint8    category  : 4;
    char     uniqueID[3];
};
#pragma pack(pop)

STATIC_ASSERT(kPdbRecordHeaderLen == sizeof(PdbRecordHeader), validPdbRecordHeader);

#define kMaxRecordSize 64*1024

struct ImageData {
    uint8 *     imgData;
    size_t      imgDataLen;
};

class MobiDoc
{
    TCHAR *             fileName;
    HANDLE              fileHandle;

    PdbHeader           pdbHeader;
    PdbRecordHeader *   recHeaders;
    char *              firstRecData;

    bool                isMobi;
    size_t              docRecCount;
    int                 compressionType;
    size_t              docUncompressedSize;

    bool                multibyte;
    size_t              trailersCount;
    size_t              imageFirstRec; // 0 if no images

    // we use bufStatic if record fits in it, bufDynamic otherwise
    char                bufStatic[kMaxRecordSize];
    char *              bufDynamic;
    size_t              bufDynamicSize;

    HuffDicDecompressor *huffDic;

    MobiDoc();

    bool    ParseHeader();
    char *  GetBufForRecordData(size_t size);
    size_t  GetRecordSize(size_t recNo);
    char*   ReadRecord(size_t recNo, size_t& sizeOut);
    bool    LoadDocRecordIntoBuffer(size_t recNo, str::Str<char>& strOut);
    void    LoadImages();
    bool    LoadImage(size_t imageNo);

public:
    str::Str<char> *    doc;

    size_t              imagesCount;
    size_t              validImagesCount;
    ImageData *         images;

    static MobiDoc *ParseFile(const TCHAR *fileName);

    char *GetBookHtmlData(size_t& lenOut) {
        lenOut = doc->Size();
        return doc->Get();
    }

    ~MobiDoc();
    bool LoadDocument();
};

#endif