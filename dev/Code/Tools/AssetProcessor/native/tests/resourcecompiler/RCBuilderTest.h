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

#pragma once

#include <AzTest/AzTest.h>
#include <qcoreapplication.h>
#include "../../utilities/assetUtils.h"
#include "../../resourcecompiler/RCBuilder.h"
#include "native/tests/AssetProcessorTest.h"

using namespace AssetProcessor;

extern const BuilderIdAndName BUILDER_ID_COPY;
extern const BuilderIdAndName BUILDER_ID_RC;
extern const BuilderIdAndName BUILDER_ID_SKIP;

class MockRCCompiler
    : public AssetProcessor::RCCompiler
{
public:
    MockRCCompiler()
        : m_executeResultResult(0, false, "c:\temp")
    {
    }

    bool Initialize(const QString& systemRoot, const QString& rcExecutableFullPath) override
    {
        m_initialize++;
        return m_initializeResult;
    }
    bool Execute(const QString& inputFile, const QString& watchFolder, int platformId, const QString& params, const QString& dest, 
        const AssetBuilderSDK::JobCancelListener* jobCancelListener, Result& result) const override
    {
        m_execute++;
        result = m_executeResultResult;
        return m_executeResult;
    }
    void RequestQuit() override
    {
        m_request_quit++;
    }

    void ResetCounters()
    {
        this->m_initialize = 0;
        this->m_execute = 0;
        this->m_request_quit = 0;
    }

    void SetResultInitialize(bool result)
    {
        m_initializeResult = result;
    }

    void SetResultExecute(bool result)
    {
        m_executeResult = result;
    }

    void SetResultResultExecute(Result result)
    {
        m_executeResultResult = result;
    }

    bool m_initializeResult = true;
    bool m_executeResult = true;
    Result m_executeResultResult;
    mutable int m_initialize = 0;
    mutable int m_execute = 0;
    mutable int m_request_quit = 0;
};


struct MockRecognizerConfiguration
    : public RecognizerConfiguration
{
    const RecognizerContainer& GetAssetRecognizerContainer() const override
    {
        return m_recognizerContainer;
    }
    const ExcludeRecognizerContainer& GetExcludeAssetRecognizerContainer() const override
    {
        return m_excludeContainer;
    }

    RecognizerContainer m_recognizerContainer;
    ExcludeRecognizerContainer m_excludeContainer;
};

struct TestInternalRecognizerBasedBuilder
    : public InternalRecognizerBasedBuilder
{
    TestInternalRecognizerBasedBuilder(RCCompiler* rcCompiler = nullptr)
        : InternalRecognizerBasedBuilder()
    {
        if (rcCompiler != nullptr)
        {
            m_rcCompiler.reset(rcCompiler);
        }
    }

    QFileInfoList GetFilesInDirectory(const QString& directoryPath) override
    {
        QFileInfoList   mockFileInfoList;
        mockFileInfoList.append(m_testFileInfo);
        return mockFileInfoList;
    }

    void TestProcessJob(const AssetBuilderSDK::ProcessJobRequest& request,
        AssetBuilderSDK::ProcessJobResponse& response)
    {
        InternalRecognizerBasedBuilder::ProcessJob(request, response);
    }

    void TestProcessLegacyRCJob(const AssetBuilderSDK::ProcessJobRequest& request,
        QString rcParam,
        AZ::Uuid productAssetType,
        const AssetBuilderSDK::JobCancelListener& jobCancelListener,
        AssetBuilderSDK::ProcessJobResponse& response)
    {
        InternalRecognizerBasedBuilder::ProcessLegacyRCJob(request, rcParam, productAssetType, jobCancelListener, response);
    }

    void TestProcessCopyJob(const AssetBuilderSDK::ProcessJobRequest& request,
        AZ::Uuid productAssetType,
        const AssetBuilderSDK::JobCancelListener& jobCancelListener,
        AssetBuilderSDK::ProcessJobResponse& response)
    {
        InternalRecognizerBasedBuilder::ProcessCopyJob(request, productAssetType, jobCancelListener, response);
    }


    TestInternalRecognizerBasedBuilder& AddTestFileInfo(const QString& testFileFullPath)
    {
        QFileInfo testFileInfo(testFileFullPath);
        m_testFileInfo.push_back(testFileInfo);
        return *this;
    }

    AZ::u32 AddTestRecognizer(QString builderID, QString extraRCParam, AssetBuilderSDK::Platform platform)
    {
        // Create a dummy test recognizer
        AssetUtilities::FilePatternMatcher patternMatcher;
        QString versionZero("0");
        AZ::Data::AssetType productAssetType = AZ::Uuid::CreateRandom();

        AssetRecognizer baseAssetRecognizer(QString("test-").append(extraRCParam), false, 1, false, false, patternMatcher, versionZero, productAssetType);

        QString platformString;
        switch (platform)
        {
        case AssetBuilderSDK::Platform::Platform_NONE:
        case AssetBuilderSDK::Platform::Platform_PC:
        default:
            platformString = "pc";
            break;
        case AssetBuilderSDK::Platform::Platform_ES3:
            platformString = "es3";
            break;
        case AssetBuilderSDK::Platform::Platform_IOS:
            platformString = "ios";
            break;
        case AssetBuilderSDK::Platform::Platform_OSX:
            platformString = "osx_gl";
            break;
        }
        QHash<QString, AssetPlatformSpec> assetPlatformSpecByPlatform;
        AssetPlatformSpec   assetSpec;
        assetSpec.m_extraRCParams = extraRCParam;
        assetPlatformSpecByPlatform[platformString] = assetSpec;

        InternalAssetRecognizer* pTestInternalRecognizer = new InternalAssetRecognizer(baseAssetRecognizer, builderID, assetPlatformSpecByPlatform);
        this->m_assetRecognizerDictionary[pTestInternalRecognizer->m_paramID] = pTestInternalRecognizer;
        return pTestInternalRecognizer->m_paramID;
    }



    QList<QFileInfo>    m_testFileInfo;
};


class RCBuilderTest
    : public AssetProcessor::AssetProcessorTest
{
    int         m_argc;
    char**      m_argv;

    QCoreApplication* m_qApp;

public:
    RCBuilderTest()
        : m_argc(0)
        , m_argv(0)
    {
        m_qApp = new QCoreApplication(m_argc, m_argv);
    }
    virtual ~RCBuilderTest()
    {
        delete m_qApp;
    }

    AZ::Uuid GetBuilderUUID() const
    {
        AZ::Uuid    rcUuid;
        AssetProcessor::BUILDER_ID_RC.GetUuid(rcUuid);
        return rcUuid;
    }

    AZStd::string GetBuilderName() const
    {
        return AZStd::string(AssetProcessor::BUILDER_ID_RC.GetName().toUtf8().data());
    }

    QString GetBuilderID() const
    {
        return AssetProcessor::BUILDER_ID_RC.GetId();
    }

    AssetBuilderSDK::ProcessJobRequest CreateTestJobRequest(const AZStd::string& testFileName, bool critical, AssetBuilderSDK::Platform platform, AZ::s64 jobId = 0)
    {
        AssetBuilderSDK::ProcessJobRequest request;
        request.m_builderGuid = this->GetBuilderUUID();
        request.m_sourceFile = testFileName;
        request.m_fullPath = AZStd::string("c:\\temp\\") + testFileName;
        request.m_tempDirPath = "c:\\temp";
        request.m_jobDescription.m_critical = critical;
        request.m_jobDescription.m_platform = platform;
        request.m_jobId = jobId;
        return request;
    }
};

