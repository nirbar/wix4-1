// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.

#include "precomp.h"


// internal function declarations

static HRESULT ParsePayloadRefsFromXml(
    __in BURN_PACKAGE* pPackage,
    __in BURN_PAYLOADS* pPayloads,
    __in IXMLDOMNode* pixnPackage
    );
static HRESULT ParsePatchTargetCode(
    __in BURN_PACKAGES* pPackages,
    __in IXMLDOMNode* pixnBundle
    );
static HRESULT FindRollbackBoundaryById(
    __in BURN_PACKAGES* pPackages,
    __in_z LPCWSTR wzId,
    __out BURN_ROLLBACK_BOUNDARY** ppRollbackBoundary
    );


// function definitions

extern "C" HRESULT PackagesParseFromXml(
    __in BURN_PACKAGES* pPackages,
    __in BURN_PAYLOADS* pPayloads,
    __in IXMLDOMNode* pixnBundle
    )
{
    HRESULT hr = S_OK;
    IXMLDOMNodeList* pixnNodes = NULL;
    IXMLDOMNode* pixnNode = NULL;
    DWORD cNodes = 0;
    BSTR bstrNodeName = NULL;
    DWORD cMspPackages = 0;
    LPWSTR scz = NULL;
    BOOL fFoundXml = FALSE;

    // select rollback boundary nodes
    hr = XmlSelectNodes(pixnBundle, L"RollbackBoundary", &pixnNodes);
    ExitOnFailure(hr, "Failed to select rollback boundary nodes.");

    // get rollback boundary node count
    hr = pixnNodes->get_length((long*)&cNodes);
    ExitOnFailure(hr, "Failed to get rollback bundary node count.");

    if (cNodes)
    {
        // allocate memory for rollback boundaries
        pPackages->rgRollbackBoundaries = (BURN_ROLLBACK_BOUNDARY*)MemAlloc(sizeof(BURN_ROLLBACK_BOUNDARY) * cNodes, TRUE);
        ExitOnNull(pPackages->rgRollbackBoundaries, hr, E_OUTOFMEMORY, "Failed to allocate memory for rollback boundary structs.");

        pPackages->cRollbackBoundaries = cNodes;

        // parse rollback boundary elements
        for (DWORD i = 0; i < cNodes; ++i)
        {
            BURN_ROLLBACK_BOUNDARY* pRollbackBoundary = &pPackages->rgRollbackBoundaries[i];

            hr = XmlNextElement(pixnNodes, &pixnNode, &bstrNodeName);
            ExitOnFailure(hr, "Failed to get next node.");

            // @Id
            hr = XmlGetAttributeEx(pixnNode, L"Id", &pRollbackBoundary->sczId);
            ExitOnRequiredXmlQueryFailure(hr, "Failed to get @Id.");

            // @Vital
            hr = XmlGetYesNoAttribute(pixnNode, L"Vital", &pRollbackBoundary->fVital);
            ExitOnRequiredXmlQueryFailure(hr, "Failed to get @Vital.");

            // prepare next iteration
            ReleaseNullObject(pixnNode);
            ReleaseNullBSTR(bstrNodeName);
        }
    }

    ReleaseNullObject(pixnNodes); // done with the RollbackBoundary elements.

    // select msi transaction nodes
    hr = XmlSelectNodes(pixnBundle, L"MsiTransaction", &pixnNodes);
    ExitOnFailure(hr, "Failed to select msi transaction nodes.");

    // get msi transaction node count
    hr = pixnNodes->get_length((long*)&cNodes);
    ExitOnFailure(hr, "Failed to get msi transaction node count.");

    if (cNodes)
    {
        // allocate memory for msi transactions
        pPackages->rgMsiTransactions = (BURN_MSI_TRANSACTION*)MemAlloc(sizeof(BURN_MSI_TRANSACTION) * cNodes, TRUE);
        ExitOnNull(pPackages->rgMsiTransactions, hr, E_OUTOFMEMORY, "Failed to allocate memory for msi transaction structs.");

        pPackages->cMsiTransactions = cNodes;

        // parse MSI transaction elements
        for (DWORD i = 0; i < cNodes; ++i)
        {
            BURN_MSI_TRANSACTION* pMsiTransaction = &pPackages->rgMsiTransactions[i];

            hr = XmlNextElement(pixnNodes, &pixnNode, &bstrNodeName);
            ExitOnFailure(hr, "Failed to get next node.");

            // @Id
            hr = XmlGetAttributeEx(pixnNode, L"Id", &pMsiTransaction->sczId);
            ExitOnRequiredXmlQueryFailure(hr, "Failed to get @Id.");

            // @LogPathVariable
            hr = XmlGetAttributeEx(pixnNode, L"LogPathVariable", &pMsiTransaction->sczLogPathVariable);
            ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @LogPathVariable.");

            // prepare next iteration
            ReleaseNullObject(pixnNode);
            ReleaseNullBSTR(bstrNodeName);
        }
    }

    ReleaseNullObject(pixnNodes); // done with the MsiTransaction elements.

    // select package nodes
    hr = XmlSelectNodes(pixnBundle, L"Chain/BundlePackage|Chain/ExePackage|Chain/MsiPackage|Chain/MspPackage|Chain/MsuPackage", &pixnNodes);
    ExitOnFailure(hr, "Failed to select package nodes.");

    // get package node count
    hr = pixnNodes->get_length((long*)&cNodes);
    ExitOnFailure(hr, "Failed to get package node count.");

    if (!cNodes)
    {
        ExitFunction1(hr = S_OK);
    }

    // allocate memory for packages
    pPackages->rgPackages = (BURN_PACKAGE*)MemAlloc(sizeof(BURN_PACKAGE) * cNodes, TRUE);
    ExitOnNull(pPackages->rgPackages, hr, E_OUTOFMEMORY, "Failed to allocate memory for package structs.");

    pPackages->cPackages = cNodes;

    // parse package elements
    for (DWORD i = 0; i < cNodes; ++i)
    {
        BURN_PACKAGE* pPackage = &pPackages->rgPackages[i];

        hr = XmlNextElement(pixnNodes, &pixnNode, &bstrNodeName);
        ExitOnFailure(hr, "Failed to get next node.");

        // @Id
        hr = XmlGetAttributeEx(pixnNode, L"Id", &pPackage->sczId);
        ExitOnRequiredXmlQueryFailure(hr, "Failed to get @Id.");

        // @Cache
        hr = XmlGetAttributeEx(pixnNode, L"Cache", &scz);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @Cache.");

        if (fFoundXml)
        {
            if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, scz, -1, L"remove", -1))
            {
                pPackage->authoredCacheType = BOOTSTRAPPER_CACHE_TYPE_REMOVE;
            }
            else if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, scz, -1, L"keep", -1))
            {
                pPackage->authoredCacheType = BOOTSTRAPPER_CACHE_TYPE_KEEP;
            }
            else if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, scz, -1, L"force", -1))
            {
                pPackage->authoredCacheType = BOOTSTRAPPER_CACHE_TYPE_FORCE;
            }
            else
            {
                hr = E_UNEXPECTED;
                ExitOnRootFailure(hr, "Invalid cache type: %ls", scz);
            }
        }

        // @CacheId
        hr = XmlGetAttributeEx(pixnNode, L"CacheId", &pPackage->sczCacheId);
        ExitOnRequiredXmlQueryFailure(hr, "Failed to get @CacheId.");

        // @Size
        hr = XmlGetAttributeUInt64(pixnNode, L"Size", &pPackage->qwSize);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @Size.");

        // @InstallSize
        hr = XmlGetAttributeUInt64(pixnNode, L"InstallSize", &pPackage->qwInstallSize);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @InstallSize.");

        // @PerMachine
        hr = XmlGetYesNoAttribute(pixnNode, L"PerMachine", &pPackage->fPerMachine);
        ExitOnRequiredXmlQueryFailure(hr, "Failed to get @PerMachine.");

        // @Permanent
        hr = XmlGetYesNoAttribute(pixnNode, L"Permanent", &pPackage->fPermanent);
        ExitOnRequiredXmlQueryFailure(hr, "Failed to get @Permanent.");

        // @Vital
        hr = XmlGetYesNoAttribute(pixnNode, L"Vital", &pPackage->fVital);
        ExitOnRequiredXmlQueryFailure(hr, "Failed to get @Vital.");

        // @LogPathVariable
        hr = XmlGetAttributeEx(pixnNode, L"LogPathVariable", &pPackage->sczLogPathVariable);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @LogPathVariable.");

        // @RollbackLogPathVariable
        hr = XmlGetAttributeEx(pixnNode, L"RollbackLogPathVariable", &pPackage->sczRollbackLogPathVariable);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @RollbackLogPathVariable.");

        if (pPackage->sczLogPathVariable && *pPackage->sczLogPathVariable)
        {
            // Format a suitable log path variable from the original package.
            hr = StrAllocFormatted(&pPackage->sczCompatibleLogPathVariable, L"%ls_Compatible", pPackage->sczLogPathVariable);
            ExitOnFailure(hr, "Failed to format log path variable for compatible package.");
        }

        // @InstallCondition
        hr = XmlGetAttributeEx(pixnNode, L"InstallCondition", &pPackage->sczInstallCondition);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @InstallCondition.");

        // @RepairCondition
        hr = XmlGetAttributeEx(pixnNode, L"RepairCondition", &pPackage->sczRepairCondition);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @RepairCondition.");

        // @RollbackBoundaryForward
        hr = XmlGetAttributeEx(pixnNode, L"RollbackBoundaryForward", &scz);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @RollbackBoundaryForward.");

        if (fFoundXml)
        {
            hr = FindRollbackBoundaryById(pPackages, scz, &pPackage->pRollbackBoundaryForward);
            ExitOnFailure(hr, "Failed to find forward transaction boundary: %ls", scz);
        }

        // @RollbackBoundaryBackward
        hr = XmlGetAttributeEx(pixnNode, L"RollbackBoundaryBackward", &scz);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @RollbackBoundaryBackward.");

        if (fFoundXml)
        {
            hr = FindRollbackBoundaryById(pPackages, scz, &pPackage->pRollbackBoundaryBackward);
            ExitOnFailure(hr, "Failed to find backward transaction boundary: %ls", scz);
        }

        // @MsiTransaction
        hr = XmlGetAttributeEx(pixnNode, L"MsiTransaction", &scz);
        ExitOnOptionalXmlQueryFailure(hr, fFoundXml, "Failed to get @MsiTransaction.");

        if (fFoundXml)
        {
            hr = PackageFindMsiTransactionById(pPackages, scz, &pPackage->pMsiTransaction);
            ExitOnFailure(hr, "Failed to find MSI transaction: %ls", scz);
        }

        // read type specific attributes
        if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, bstrNodeName, -1, L"BundlePackage", -1))
        {
            pPackage->type = BURN_PACKAGE_TYPE_BUNDLE;

            hr = BundlePackageEngineParsePackageFromXml(pixnNode, pPackage); // TODO: Modularization
            ExitOnFailure(hr, "Failed to parse BUNDLE package.");
        }
        else if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, bstrNodeName, -1, L"ExePackage", -1))
        {
            pPackage->type = BURN_PACKAGE_TYPE_EXE;

            hr = ExeEngineParsePackageFromXml(pixnNode, pPackage); // TODO: Modularization
            ExitOnFailure(hr, "Failed to parse EXE package.");
        }
        else if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, bstrNodeName, -1, L"MsiPackage", -1))
        {
            pPackage->type = BURN_PACKAGE_TYPE_MSI;

            hr = MsiEngineParsePackageFromXml(pixnNode, pPackage); // TODO: Modularization
            ExitOnFailure(hr, "Failed to parse MSI package.");
        }
        else if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, bstrNodeName, -1, L"MspPackage", -1))
        {
            pPackage->type = BURN_PACKAGE_TYPE_MSP;

            hr = MspEngineParsePackageFromXml(pixnNode, pPackage); // TODO: Modularization
            ExitOnFailure(hr, "Failed to parse MSP package.");

            ++cMspPackages;
        }
        else if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, bstrNodeName, -1, L"MsuPackage", -1))
        {
            pPackage->type = BURN_PACKAGE_TYPE_MSU;

            hr = MsuEngineParsePackageFromXml(pixnNode, pPackage); // TODO: Modularization
            ExitOnFailure(hr, "Failed to parse MSU package.");
        }
        else
        {
            ExitWithRootFailure(hr, E_UNEXPECTED, "Invalid package type: %ls", bstrNodeName);
        }

        if (!pPackage->fPermanent)
        {
            BOOL fUninstallable = TRUE;

            switch (pPackage->type)
            {
            case BURN_PACKAGE_TYPE_EXE:
                fUninstallable = pPackage->Exe.fUninstallable;
                break;
            case BURN_PACKAGE_TYPE_MSU:
                fUninstallable = FALSE;
                break;
            }

            if (!fUninstallable)
            {
                ExitWithRootFailure(hr, E_INVALIDDATA, "Non-permanent packages must be uninstallable.");
            }
        }

        pPackage->fCanAffectRegistration = !pPackage->fPermanent;

        // parse payload references
        hr = ParsePayloadRefsFromXml(pPackage, pPayloads, pixnNode);
        ExitOnFailure(hr, "Failed to parse payload references.");

        // parse dependency providers
        hr = DependencyParseProvidersFromXml(pPackage, pixnNode);
        ExitOnFailure(hr, "Failed to parse dependency providers.");

        // prepare next iteration
        ReleaseNullObject(pixnNode);
        ReleaseNullBSTR(bstrNodeName);
    }

    if (cMspPackages)
    {
        pPackages->rgPatchInfo = static_cast<MSIPATCHSEQUENCEINFOW*>(MemAlloc(sizeof(MSIPATCHSEQUENCEINFOW) * cMspPackages, TRUE));
        ExitOnNull(pPackages->rgPatchInfo, hr, E_OUTOFMEMORY, "Failed to allocate memory for MSP patch sequence information.");

        pPackages->rgPatchInfoToPackage = static_cast<BURN_PACKAGE**>(MemAlloc(sizeof(BURN_PACKAGE*) * cMspPackages, TRUE));
        ExitOnNull(pPackages->rgPatchInfoToPackage, hr, E_OUTOFMEMORY, "Failed to allocate memory for patch sequence information to package lookup.");

        for (DWORD i = 0; i < pPackages->cPackages; ++i)
        {
            BURN_PACKAGE* pPackage = &pPackages->rgPackages[i];

            if (BURN_PACKAGE_TYPE_MSP == pPackage->type)
            {
                pPackages->rgPatchInfo[pPackages->cPatchInfo].szPatchData = pPackage->Msp.sczApplicabilityXml;
                pPackages->rgPatchInfo[pPackages->cPatchInfo].ePatchDataType = MSIPATCH_DATATYPE_XMLBLOB;
                pPackages->rgPatchInfoToPackage[pPackages->cPatchInfo] = pPackage;
                ++pPackages->cPatchInfo;

                // Loop through all MSI packages seeing if any of them slipstream this MSP.
                for (DWORD j = 0; j < pPackages->cPackages; ++j)
                {
                    BURN_PACKAGE* pMsiPackage = &pPackages->rgPackages[j];

                    if (BURN_PACKAGE_TYPE_MSI == pMsiPackage->type)
                    {
                        for (DWORD k = 0; k < pMsiPackage->Msi.cSlipstreamMspPackages; ++k)
                        {
                            if (pMsiPackage->Msi.rgsczSlipstreamMspPackageIds[k] && CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, pPackage->sczId, -1, pMsiPackage->Msi.rgsczSlipstreamMspPackageIds[k], -1))
                            {
                                BURN_SLIPSTREAM_MSP* pSlipstreamMsp = pMsiPackage->Msi.rgSlipstreamMsps + k;
                                pSlipstreamMsp->pMspPackage = pPackage;
                                pSlipstreamMsp->dwMsiChainedPatchIndex = BURN_PACKAGE_INVALID_PATCH_INDEX;

                                ReleaseNullStr(pMsiPackage->Msi.rgsczSlipstreamMspPackageIds[k]); // we don't need the slipstream package id any longer so free it.
                            }
                        }
                    }
                }
            }
        }
    }

    AssertSz(pPackages->cPatchInfo == cMspPackages, "Count of packages patch info should be equal to the number of MSP packages.");

#if DEBUG
    // Loop through all MSI packages seeing if any of them are missing their slipstream MSP.
    for (DWORD i = 0; i < pPackages->cPackages; ++i)
    {
        BURN_PACKAGE* pPackage = &pPackages->rgPackages[i];

        if (BURN_PACKAGE_TYPE_MSI == pPackage->type)
        {
            for (DWORD k = 0; k < pPackage->Msi.cSlipstreamMspPackages; ++k)
            {
                if (pPackage->Msi.rgsczSlipstreamMspPackageIds[k])
                {
                    AssertSz(FALSE, "MSI slipstream MSP package doesn't exist.");
                }
            }
        }
    }
#endif

    hr = ParsePatchTargetCode(pPackages, pixnBundle);
    ExitOnFailure(hr, "Failed to parse target product codes.");

    hr = S_OK;

LExit:
    ReleaseObject(pixnNodes);
    ReleaseObject(pixnNode);
    ReleaseBSTR(bstrNodeName);
    ReleaseStr(scz);

    return hr;
}

extern "C" void PackageUninitialize(
    __in BURN_PACKAGE* pPackage
    )
{
    ReleaseStr(pPackage->sczId);
    ReleaseStr(pPackage->sczLogPathVariable);
    ReleaseStr(pPackage->sczRollbackLogPathVariable);
    ReleaseStr(pPackage->sczCompatibleLogPathVariable);
    ReleaseStr(pPackage->sczInstallCondition);
    ReleaseStr(pPackage->sczRepairCondition);
    ReleaseStr(pPackage->sczCacheId);

    if (pPackage->rgDependencyProviders)
    {
        for (DWORD i = 0; i < pPackage->cDependencyProviders; ++i)
        {
            DependencyUninitializeProvider(pPackage->rgDependencyProviders + i);
        }
        MemFree(pPackage->rgDependencyProviders);
    }

    ReleaseMem(pPackage->payloads.rgItems);

    switch (pPackage->type)
    {
    case BURN_PACKAGE_TYPE_BUNDLE:
        BundlePackageEnginePackageUninitialize(pPackage); // TODO: Modularization
        break;
    case BURN_PACKAGE_TYPE_EXE:
        ExeEnginePackageUninitialize(pPackage); // TODO: Modularization
        break;
    case BURN_PACKAGE_TYPE_MSI:
        MsiEnginePackageUninitialize(pPackage); // TODO: Modularization
        break;
    case BURN_PACKAGE_TYPE_MSP:
        MspEnginePackageUninitialize(pPackage); // TODO: Modularization
        break;
    case BURN_PACKAGE_TYPE_MSU:
        MsuEnginePackageUninitialize(pPackage); // TODO: Modularization
        break;
    }

    PackageUninitializeCompatible(&pPackage->compatiblePackage);
}

extern "C" void PackageUninitializeCompatible(
    __in BURN_COMPATIBLE_PACKAGE* pCompatiblePackage
    )
{
    ReleaseStr(pCompatiblePackage->compatibleEntry.sczId);
    ReleaseStr(pCompatiblePackage->compatibleEntry.sczName);
    ReleaseStr(pCompatiblePackage->compatibleEntry.sczProviderKey);
    ReleaseStr(pCompatiblePackage->compatibleEntry.sczVersion);

    ReleaseStr(pCompatiblePackage->sczCacheId);

    switch (pCompatiblePackage->type)
    {
    case BURN_PACKAGE_TYPE_MSI:
        ReleaseStr(pCompatiblePackage->Msi.sczVersion);
        ReleaseVerutilVersion(pCompatiblePackage->Msi.pVersion);
        break;
    }

    // clear struct
    memset(pCompatiblePackage, 0, sizeof(BURN_COMPATIBLE_PACKAGE));
}

extern "C" void PackagesUninitialize(
    __in BURN_PACKAGES* pPackages
    )
{
    if (pPackages->rgRollbackBoundaries)
    {
        for (DWORD i = 0; i < pPackages->cRollbackBoundaries; ++i)
        {
            ReleaseStr(pPackages->rgRollbackBoundaries[i].sczId);
        }
        MemFree(pPackages->rgRollbackBoundaries);
    }

    if (pPackages->rgMsiTransactions)
    {
        for (DWORD i = 0; i < pPackages->cMsiTransactions; ++i)
        {
            ReleaseStr(pPackages->rgMsiTransactions[i].sczId);
            ReleaseStr(pPackages->rgMsiTransactions[i].sczLogPath);
            ReleaseStr(pPackages->rgMsiTransactions[i].sczLogPathVariable);
        }
        MemFree(pPackages->rgMsiTransactions);
    }

    if (pPackages->rgPackages)
    {
        for (DWORD i = 0; i < pPackages->cPackages; ++i)
        {
            PackageUninitialize(pPackages->rgPackages + i);
        }
        MemFree(pPackages->rgPackages);
    }

    if (pPackages->rgPatchTargetCodes)
    {
        for (DWORD i = 0; i < pPackages->cPatchTargetCodes; ++i)
        {
            ReleaseStr(pPackages->rgPatchTargetCodes[i].sczTargetCode);
        }
        MemFree(pPackages->rgPatchTargetCodes);
    }

    ReleaseMem(pPackages->rgPatchInfo);
    ReleaseMem(pPackages->rgPatchInfoToPackage);

    // clear struct
    memset(pPackages, 0, sizeof(BURN_PACKAGES));
}

extern "C" HRESULT PackageFindById(
    __in BURN_PACKAGES* pPackages,
    __in_z LPCWSTR wzId,
    __out BURN_PACKAGE** ppPackage
    )
{
    HRESULT hr = S_OK;
    BURN_PACKAGE* pPackage = NULL;

    for (DWORD i = 0; i < pPackages->cPackages; ++i)
    {
        pPackage = &pPackages->rgPackages[i];

        if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, pPackage->sczId, -1, wzId, -1))
        {
            *ppPackage = pPackage;
            ExitFunction1(hr = S_OK);
        }
    }

    hr = E_NOTFOUND;

LExit:
    return hr;
}


extern "C" HRESULT PackageFindRelatedById(
    __in BURN_RELATED_BUNDLES* pRelatedBundles,
    __in_z LPCWSTR wzId,
    __out BURN_PACKAGE** ppPackage
    )
{
    HRESULT hr = S_OK;
    BURN_RELATED_BUNDLE* pRelatedBundle = NULL;

    hr = RelatedBundleFindById(pRelatedBundles, wzId, &pRelatedBundle);
    *ppPackage = FAILED(hr) ? NULL : &pRelatedBundle->package;

    return hr;
}

/********************************************************************
 PackageGetProperty - Determines if the property is defined
  and optionally copies the property value.

 Note: The caller must free psczValue if requested.

 Note: Returns E_NOTFOUND if the property was not defined or if the
  package does not support properties.

*********************************************************************/
extern "C" HRESULT PackageGetProperty(
    __in const BURN_PACKAGE* pPackage,
    __in_z LPCWSTR wzProperty,
    __out_z_opt LPWSTR* psczValue
    )
{
    HRESULT hr = E_NOTFOUND;
    BURN_MSIPROPERTY* rgProperties = NULL;
    DWORD cProperties = 0;

    // For MSIs and MSPs, enumerate the properties looking for wzProperty.
    if (BURN_PACKAGE_TYPE_MSI == pPackage->type)
    {
        rgProperties = pPackage->Msi.rgProperties;
        cProperties = pPackage->Msi.cProperties;
    }
    else if (BURN_PACKAGE_TYPE_MSP == pPackage->type)
    {
        rgProperties = pPackage->Msp.rgProperties;
        cProperties = pPackage->Msp.cProperties;
    }

    for (DWORD i = 0; i < cProperties; ++i)
    {
        const BURN_MSIPROPERTY* pProperty = &rgProperties[i];

        if (CSTR_EQUAL == ::CompareStringW(LOCALE_NEUTRAL, 0, pProperty->sczId, -1, wzProperty, -1))
        {
            if (psczValue)
            {
                hr = StrAllocString(psczValue, pProperty->sczValue, 0);
                ExitOnFailure(hr, "Failed to copy the property value.");
            }

            ExitFunction1(hr = S_OK);
        }
    }

LExit:
    return hr;
}

extern "C" HRESULT PackageFindMsiTransactionById(
    __in BURN_PACKAGES* pPackages,
    __in_z LPCWSTR wzId,
    __out BURN_MSI_TRANSACTION** ppTransaction
    )
{
    HRESULT hr = S_OK;
    BURN_MSI_TRANSACTION* pMsiTransaction = NULL;

    for (DWORD i = 0; i < pPackages->cMsiTransactions; ++i)
    {
        pMsiTransaction = &pPackages->rgMsiTransactions[i];

        if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, pMsiTransaction->sczId, -1, wzId, -1))
        {
            *ppTransaction = pMsiTransaction;
            ExitFunction1(hr = S_OK);
        }
    }

    hr = E_NOTFOUND;

LExit:
    return hr;
}


// internal function declarations

static HRESULT ParsePayloadRefsFromXml(
    __in BURN_PACKAGE* pPackage,
    __in BURN_PAYLOADS* pPayloads,
    __in IXMLDOMNode* pixnPackage
    )
{
    HRESULT hr = S_OK;
    IXMLDOMNodeList* pixnNodes = NULL;
    IXMLDOMNode* pixnNode = NULL;
    DWORD cNodes = 0;
    LPWSTR sczId = NULL;

    // select package nodes
    hr = XmlSelectNodes(pixnPackage, L"PayloadRef", &pixnNodes);
    ExitOnFailure(hr, "Failed to select package nodes.");

    // get package node count
    hr = pixnNodes->get_length((long*)&cNodes);
    ExitOnFailure(hr, "Failed to get package node count.");

    if (!cNodes)
    {
        ExitFunction1(hr = S_OK);
    }

    // allocate memory for payload pointers
    pPackage->payloads.rgItems = (BURN_PAYLOAD_GROUP_ITEM*)MemAlloc(sizeof(BURN_PAYLOAD_GROUP_ITEM) * cNodes, TRUE);
    ExitOnNull(pPackage->payloads.rgItems, hr, E_OUTOFMEMORY, "Failed to allocate memory for package payloads.");

    pPackage->payloads.cItems = cNodes;

    // parse package elements
    for (DWORD i = 0; i < cNodes; ++i)
    {
        BURN_PAYLOAD_GROUP_ITEM* pPackagePayload = pPackage->payloads.rgItems + i;

        hr = XmlNextElement(pixnNodes, &pixnNode, NULL);
        ExitOnFailure(hr, "Failed to get next node.");

        // @Id
        hr = XmlGetAttributeEx(pixnNode, L"Id", &sczId);
        ExitOnFailure(hr, "Failed to get Id attribute.");

        // find payload
        hr = PayloadFindById(pPayloads, sczId, &pPackagePayload->pPayload);
        ExitOnFailure(hr, "Failed to find payload.");

        pPackage->payloads.qwTotalSize += pPackagePayload->pPayload->qwFileSize;

        // prepare next iteration
        ReleaseNullObject(pixnNode);
    }

    hr = S_OK;

LExit:
    ReleaseObject(pixnNodes);
    ReleaseObject(pixnNode);
    ReleaseStr(sczId);

    return hr;
}

static HRESULT ParsePatchTargetCode(
    __in BURN_PACKAGES* pPackages,
    __in IXMLDOMNode* pixnBundle
    )
{
    HRESULT hr = S_OK;
    IXMLDOMNodeList* pixnNodes = NULL;
    IXMLDOMNode* pixnNode = NULL;
    DWORD cNodes = 0;
    BOOL fProduct;

    hr = XmlSelectNodes(pixnBundle, L"PatchTargetCode", &pixnNodes);
    ExitOnFailure(hr, "Failed to select PatchTargetCode nodes.");

    hr = pixnNodes->get_length((long*)&cNodes);
    ExitOnFailure(hr, "Failed to get PatchTargetCode node count.");

    if (!cNodes)
    {
        ExitFunction1(hr = S_OK);
    }

    pPackages->rgPatchTargetCodes = (BURN_PATCH_TARGETCODE*)MemAlloc(sizeof(BURN_PATCH_TARGETCODE) * cNodes, TRUE);
    ExitOnNull(pPackages->rgPatchTargetCodes, hr, E_OUTOFMEMORY, "Failed to allocate memory for patch targetcodes.");

    pPackages->cPatchTargetCodes = cNodes;

    for (DWORD i = 0; i < cNodes; ++i)
    {
        BURN_PATCH_TARGETCODE* pTargetCode = pPackages->rgPatchTargetCodes + i;

        hr = XmlNextElement(pixnNodes, &pixnNode, NULL);
        ExitOnFailure(hr, "Failed to get next node.");

        hr = XmlGetAttributeEx(pixnNode, L"TargetCode", &pTargetCode->sczTargetCode);
        ExitOnFailure(hr, "Failed to get @TargetCode attribute.");

        hr = XmlGetYesNoAttribute(pixnNode, L"Product", &fProduct);
        if (E_NOTFOUND == hr)
        {
            fProduct = FALSE;
            hr = S_OK;
        }
        ExitOnFailure(hr, "Failed to get @Product.");

        pTargetCode->type = fProduct ? BURN_PATCH_TARGETCODE_TYPE_PRODUCT : BURN_PATCH_TARGETCODE_TYPE_UPGRADE;

        // prepare next iteration
        ReleaseNullObject(pixnNode);
    }

LExit:
    ReleaseObject(pixnNode);
    ReleaseObject(pixnNodes);

    return hr;
}

static HRESULT FindRollbackBoundaryById(
    __in BURN_PACKAGES* pPackages,
    __in_z LPCWSTR wzId,
    __out BURN_ROLLBACK_BOUNDARY** ppRollbackBoundary
    )
{
    HRESULT hr = S_OK;
    BURN_ROLLBACK_BOUNDARY* pRollbackBoundary = NULL;

    for (DWORD i = 0; i < pPackages->cRollbackBoundaries; ++i)
    {
        pRollbackBoundary = &pPackages->rgRollbackBoundaries[i];

        if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, 0, pRollbackBoundary->sczId, -1, wzId, -1))
        {
            *ppRollbackBoundary = pRollbackBoundary;
            ExitFunction1(hr = S_OK);
        }
    }

    hr = E_NOTFOUND;

LExit:
    return hr;
}
