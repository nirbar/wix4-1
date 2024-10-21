<img src="https://github.com/wixtoolset/Home/raw/master/imgs/wix-white-bg.png" alt="WiX Toolset" height="128" />

[![latest version](https://img.shields.io/nuget/vpre/wix)](https://www.nuget.org/packages/PanelSwWix4)
[![download count](https://img.shields.io/nuget/dt/wix)](https://www.nuget.org/stats/packages/PanelSwWix4?groupby=Version)
[![build status](https://img.shields.io/github/actions/workflow/status/nirbar/wix4/build.yml?branch=develop)](https://github.com/wixtoolset/wix/actions/workflows/build.yml?query=branch%3Adevelop)

# PanelSwWix4: Custom WiX Toolset

This repository contains the PanelSwWix4: A custom WiX Toolset codebase

# Customizations:

- [Bundle/@RunAsAdmin](https://github.com/wixtoolset/issues/issues/5309): Support starting the bootstrapper elevated
- [Harvest AdditionalOptions](https://github.com/wixtoolset/issues/issues/7427): Allow customized AdditionalOptions for each HarvestX MSBuild Item
- [Fix MultiSzInsertString](https://github.com/wixtoolset/issues/issues/7311) to create a double-null terminated buffer
- [5220](https://github.com/wixtoolset/issues/issues/5220): Automatically add logging flag for any burn ExePackage, BundlePackage, and related bundles
- [6298](https://github.com/wixtoolset/issues/issues/6298): Extract detached containers with "wix burn extract"
- [6252](https://github.com/wixtoolset/issues/issues/6252): Automatically add -norestart flag for any burn ExePackage, BundlePackage, and related bundles
- [7552](https://github.com/wixtoolset/issues/issues/7552): Add burn command line argument to log to console: /clog or /conlog
- [7877](https://github.com/wixtoolset/issues/issues/7877): ArpEntry reads QuietUninstallString or UninstallString, and uses UninstallArguments for the uninstall command line
- ExePackage/@DetectVersionVariable: Support using one of the XxxSearch elements to provide a version number in a variable to test against the package's version.
- [7778](https://github.com/wixtoolset/issues/issues/7778): Bundle/@Wix3DependencyMode=yes/no. If yes, then default MSI/MSP provider key uses the ProductCode/PatchCode. Use only if a WiX4 bundle chains packages of a WiX3 bundle.
- [4889](https://github.com/wixtoolset/issues/issues/4889): Support custom container compressions using bundle extensions
- Not overwriting log files when retrying to execute a package
- Support sending custom messages on embedded pipe
- Best effort to log premature termination of companion process
- Monitor UX folder and re-extract any UX payloads that were deleted for any reason
