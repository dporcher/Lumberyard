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

#include "StdAfx.h"
#include "UICanvasAssetTypeInfo.h"

namespace LmbrCentral
{
    static AZ::Data::AssetType uiCanvasAssetType("{E48DDAC8-1F1E-4183-AAAB-37424BCC254B}");

    UICanvasAssetTypeInfo::~UICanvasAssetTypeInfo()
    {
        Unregister();
    }

    void UICanvasAssetTypeInfo::Register()
    {
        AZ::AssetTypeInfoBus::Handler::BusConnect(uiCanvasAssetType);
    }

    void UICanvasAssetTypeInfo::Unregister()
    {
        AZ::AssetTypeInfoBus::Handler::BusDisconnect(uiCanvasAssetType);
    }

    AZ::Data::AssetType UICanvasAssetTypeInfo::GetAssetType() const
    {
        return uiCanvasAssetType;
    }

    const char* UICanvasAssetTypeInfo::GetAssetTypeDisplayName() const
    {
        return "UI Canvas";
    }

    const char* UICanvasAssetTypeInfo::GetGroup() const
    {
        return "UI";
    }

    const char * UICanvasAssetTypeInfo::GetBrowserIcon() const
    {
        return "Editor/Icons/Components/UiCanvasRef.png";
    }
} // namespace LmbrCentral
