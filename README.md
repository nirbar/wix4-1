<img src="https://github.com/wixtoolset/Home/raw/master/imgs/wix-white-bg.png" alt="WiX Toolset" height="128" />

[![latest version](https://img.shields.io/nuget/vpre/wix)](https://www.nuget.org/packages/PanelSwWix4)
[![download count](https://img.shields.io/nuget/dt/wix)](https://www.nuget.org/stats/packages/PanelSwWix4?groupby=Version)
[![build status](https://img.shields.io/github/actions/workflow/status/nirbar/wix4/build.yml?branch=develop)](https://github.com/wixtoolset/wix/actions/workflows/build.yml?query=branch%3Adevelop)

# PanelSwWix4: Custom WiX Toolset

This repository contains the PanelSwWix4: A custom WiX Toolset codebase

# Customizations:

- [Fix MultiSzInsertString](https://github.com/wixtoolset/issues/issues/7311) to create a double-null terminated buffer
- [6298](https://github.com/wixtoolset/issues/issues/6298): Extract detached containers with "wix burn extract"
- ExePackage/@DetectVersionVariable: Support using one of the XxxSearch elements to provide a version number in a variable to test against the package's version.
- [4889](https://github.com/wixtoolset/issues/issues/4889): Support custom container compressions using bundle extensions
- Un-plan MSI transaction if it contains less than 2 packages
- Add BA methods OnPlanMsiTransaction, OnPlanMsiTransactionComplete: Let BA opt-out of MSI transactions.
- Not overwriting log files when retrying to execute a package
- Support sending custom messages on embedded pipe
- Best effort to log premature termination of companion process
- [Bundle/@RunAsAdmin](https://github.com/wixtoolset/issues/issues/5309) attribute creates a bootstrapper that requires elevation when launched
- Monitor UX folder and re-extract any UX payloads that were deleted for any reason
- Reorder cache actions: moves non-executing package caching to the end of the plan. This optimizes run time as packages that are cached but not executed will not stand in the way of executing packages.
- [8896](https://github.com/wixtoolset/issues/issues/8896): Component/@WiX3CompatibleGuid- Support WiX3-compantible auto-guids for registry key path
- [8962](https://github.com/wixtoolset/issues/issues/8962): ForceAbsent means a package can be removed even if it has other dependants
