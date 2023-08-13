// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.

namespace WixToolsetTest.CoreIntegration
{
    using System.Collections.Generic;
    using System.IO;
    using WixInternal.TestSupport;
    using WixInternal.Core.TestPackage;
    using Xunit;

    public class MsiTransactionFixture
    {
        [Fact]
        public void TwoTransactionsBundle()
        {
            var folder = TestData.Get(@"TestData");

            using (var fs = new DisposableFileSystem())
            {
                var baseFolder = fs.GetFolder();
                var intermediateFolder = Path.Combine(baseFolder, "obj");
                var binFolder = Path.Combine(baseFolder, "bin");
                var exePath = Path.Combine(baseFolder, @"bin\test.exe");

                BuildMsiPackages(folder, intermediateFolder, binFolder);

                var result = WixRunner.Execute(new[]
                {
                    "build",
                    "-sw1151", // this is expected for this test
                    Path.Combine(folder, "MsiTransaction", "TwoTransactionsBundle.wxs"),
                    Path.Combine(folder, "BundleWithPackageGroupRef", "Bundle.wxs"),
                    "-bindpath", Path.Combine(folder, "SimpleBundle", "data"),
                    "-bindpath", Path.Combine(folder, ".Data"),
                    "-bindpath", binFolder,
                    "-intermediateFolder", intermediateFolder,
                    "-o", exePath,
                });

                Assert.Equal(0, result.ExitCode);
            }
        }

        [Fact]
        public void CannotBuildExePackageInMsiTransaction()
        {
            var folder = TestData.Get(@"TestData");

            using (var fs = new DisposableFileSystem())
            {
                var baseFolder = fs.GetFolder();
                var intermediateFolder = Path.Combine(baseFolder, "obj");
                var exePath = Path.Combine(baseFolder, @"bin\test.exe");

                var result = WixRunner.Execute(new[]
                {
                    "build",
                    "-sw1151", // this is expected for this test
                    Path.Combine(folder, "MsiTransaction", "ExeInMsiTransactionBundle.wxs"),
                    Path.Combine(folder, "BundleWithPackageGroupRef", "Bundle.wxs"),
                    "-bindpath", Path.Combine(folder, "SimpleBundle", "data"),
                    "-bindpath", Path.Combine(folder, ".Data"),
                    "-intermediateFolder", intermediateFolder,
                    "-o", exePath,
                });

                Assert.Equal(5, result.ExitCode);
            }
        }

        [Fact]
        public void CannotBuildExePackageRefInMsiTransaction()
        {
            var folder = TestData.Get(@"TestData");

            using (var fs = new DisposableFileSystem())
            {
                var baseFolder = fs.GetFolder();
                var intermediateFolder = Path.Combine(baseFolder, "obj");
                var exePath = Path.Combine(baseFolder, @"bin\test.exe");

                var result = WixRunner.Execute(new[]
                {
                    "build",
                    "-sw1151", // this is expected for this test
                    Path.Combine(folder, "MsiTransaction", "ExeRefInMsiTransactionBundle.wxs"),
                    Path.Combine(folder, "BundleWithPackageGroupRef", "Bundle.wxs"),
                    "-bindpath", Path.Combine(folder, "SimpleBundle", "data"),
                    "-bindpath", Path.Combine(folder, ".Data"),
                    "-intermediateFolder", intermediateFolder,
                    "-o", exePath,
                });

                Assert.Equal(412, result.ExitCode);
            }
        }

        [Fact]
        public void CannotBuildNestedMsiTransaction()
        {
            var folder = TestData.Get(@"TestData");

            using (var fs = new DisposableFileSystem())
            {
                var baseFolder = fs.GetFolder();
                var intermediateFolder = Path.Combine(baseFolder, "obj");
                var binFolder = Path.Combine(baseFolder, "bin");
                var exePath = Path.Combine(baseFolder, @"bin\test.exe");

                BuildMsiPackages(folder, intermediateFolder, binFolder);

                var result = WixRunner.Execute(new[]
                {
                    "build",
                    "-sw1151", // this is expected for this test
                    Path.Combine(folder, "MsiTransaction", "NestedMsiTransaction.wxs"),
                    Path.Combine(folder, "BundleWithPackageGroupRef", "Bundle.wxs"),
                    "-bindpath", Path.Combine(folder, "SimpleBundle", "data"),
                    "-bindpath", Path.Combine(folder, ".Data"),
                    "-bindpath", binFolder,
                    "-intermediateFolder", intermediateFolder,
                    "-o", exePath,
                });

                Assert.Equal(412, result.ExitCode);
            }
        }

        [Fact]
        public void CannotBuildX64AfterX86Bundle()
        {
            var folder = TestData.Get(@"TestData");

            using (var fs = new DisposableFileSystem())
            {
                var baseFolder = fs.GetFolder();
                var intermediateFolder = Path.Combine(baseFolder, "obj");
                var binFolder = Path.Combine(baseFolder, "bin");
                var exePath = Path.Combine(binFolder, "test.exe");

                BuildMsiPackages(folder, intermediateFolder, binFolder);

                var result = WixRunner.Execute(new[]
                {
                    "build",
                    "-sw1151", // this is expected for this test
                    Path.Combine(folder, "MsiTransaction", "X64AfterX86Bundle.wxs"),
                    Path.Combine(folder, "BundleWithPackageGroupRef", "Bundle.wxs"),
                    "-bindpath", Path.Combine(folder, "SimpleBundle", "data"),
                    "-bindpath", binFolder,
                    "-intermediateFolder", intermediateFolder,
                    "-o", exePath,
                });

                Assert.Equal(418, result.ExitCode);
            }
        }

        [Fact]
        public void CannotBuildX86AfterX64Bundle()
        {
            var folder = TestData.Get(@"TestData");

            using (var fs = new DisposableFileSystem())
            {
                var baseFolder = fs.GetFolder();
                var intermediateFolder = Path.Combine(baseFolder, "obj");
                var binFolder = Path.Combine(baseFolder, "bin");
                var exePath = Path.Combine(binFolder, "test.exe");
                var baFolderPath = Path.Combine(baseFolder, "ba");
                var extractFolderPath = Path.Combine(baseFolder, "extract");

                BuildMsiPackages(folder, intermediateFolder, binFolder);

                var result = WixRunner.Execute(new[]
                {
                    "build",
                    "-sw1151", // this is expected for this test
                    Path.Combine(folder, "MsiTransaction", "X86AfterX64Bundle.wxs"),
                    Path.Combine(folder, "BundleWithPackageGroupRef", "Bundle.wxs"),
                    "-bindpath", Path.Combine(folder, "SimpleBundle", "data"),
                    "-bindpath", binFolder,
                    "-intermediateFolder", intermediateFolder,
                    "-o", exePath,
                });

                Assert.Equal(418, result.ExitCode);
            }
        }

        private static void BuildMsiPackages(string folder, string intermediateFolder, string binFolder)
        {
            var result = WixRunner.Execute(new[]
            {
                "build",
                Path.Combine(folder, "MsiTransaction", "FirstX86.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "MinimalComponentGroup.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "Product.wxs"),
                "-bindpath", Path.Combine(folder, "SingleFile", "data"),
                "-intermediateFolder", intermediateFolder,
                "-o", Path.Combine(binFolder, "FirstX86", "FirstX86.msi"),
            });

            result.AssertSuccess();

            result = WixRunner.Execute(new[]
            {
                "build",
                Path.Combine(folder, "MsiTransaction", "SecondX86.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "MinimalComponentGroup.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "Product.wxs"),
                "-bindpath", Path.Combine(folder, "SingleFile", "data"),
                "-intermediateFolder", intermediateFolder,
                "-o", Path.Combine(binFolder, "SecondX86", "SecondX86.msi"),
            });

            result.AssertSuccess();

            result = WixRunner.Execute(new[]
            {
                "build",
                Path.Combine(folder, "MsiTransaction", "FirstX64.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "MinimalComponentGroup.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "Product.wxs"),
                "-bindpath", Path.Combine(folder, "SingleFile", "data"),
                "-intermediateFolder", intermediateFolder,
                "-arch", "x64",
                "-o", Path.Combine(binFolder, "FirstX64", "FirstX64.msi"),
            });

            result.AssertSuccess();

            result = WixRunner.Execute(new[]
            {
                "build",
                Path.Combine(folder, "MsiTransaction", "SecondX64.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "MinimalComponentGroup.wxs"),
                Path.Combine(folder, "ProductWithComponentGroupRef", "Product.wxs"),
                "-bindpath", Path.Combine(folder, "SingleFile", "data"),
                "-intermediateFolder", intermediateFolder,
                "-arch", "x64",
                "-o", Path.Combine(binFolder, "SecondX64", "SecondX64.msi"),
            });

            result.AssertSuccess();
        }
    }
}
