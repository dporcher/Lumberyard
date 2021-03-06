/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#ifndef _TEXTMESSAGES_H_
#define _TEXTMESSAGES_H_

// compact buffer to store text messages for a frame and render them each frame
// (replacement for the former PodArray<text_info_struct> m_listMessages[2], cleaner/more cache friendly,less memory,faster,typesafe)
// todo: can release memory in case this is needed
class CTextMessages
{
public:
    CTextMessages()
        : m_dwCurrentReadPos() {}

    class CTextMessageHeader;

    // iteration should not be started yet
    // Arguments
    //   vPos - WorldSpace position
    //   szText - must not be 0
    //   nDrawFlags - EDrawTextFlags
    void PushEntry_Text(const Vec3& vPos, const ColorB col, const float fFontSize, const int nDrawFlags, const char* szText);

    // usually called every frame
    // resets/ends iteration
    void Clear(bool posonly = false);

    // todo: improve interface
    // starts the iteration
    // Returns
    //   0 if there are no more entries
    const CTextMessageHeader* GetNextEntry();

    uint32 ComputeSizeInMemory() const;

    //
    bool empty() const { return m_TextMessageData.empty(); }

    // -------------------------------------------------------------

    struct SText;

    class CTextMessageHeader
    {
    public:
        const SText* CastTo_Text() const { return (SText*)this; }

        uint16 GetSize() const { return m_Size; }

    protected: // ---------------------------------------------
        uint16      m_Size; // including attached text
    };

    // ---------------------------------------------

    struct SText
        : public CTextMessageHeader
    {
        void Init(const uint32 paddedSize) { assert(paddedSize < 65535); m_Size = paddedSize; }
        const char* GetText() const { return (char*)this + sizeof(*this); }

        Vec3            m_vPos;
        ColorB      m_Color;
        float           m_fFontSize;
        uint32      m_nDrawFlags;       // EDrawTextFlags
    };

    void GetMemoryUsage(ICrySizer* pSizer) const
    {
        pSizer->AddObject(m_TextMessageData);
    }
private: // ------------------------------------------------------

    // each call the former returned pointer might be invalid
    // Arguments
    //   dwBytes >0 and dividable by 4
    // Returns
    //   0 if there is not enough space
    uint8* PushData(const uint32 dwBytes);

    // ------------------------------------------------------

    std::vector<uint8>              m_TextMessageData;      // consists of many 4 byte aligned STextMessageHeader+ZeroTermintedText
    uint32                                      m_dwCurrentReadPos;     // in bytes, !=0 interation started

    CryCriticalSection              m_TextMessageLock;
};


#endif // #ifndef _TEXTMESSAGES_H_
