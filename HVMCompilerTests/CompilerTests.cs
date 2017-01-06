using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Diagnostics;
using HVMCompiler;

namespace HVMCompilerTests
{
    /// <summary>
    /// Summary description for UnitTest1
    /// </summary>
    [TestClass]
    public class CompilerTests
    {
        public CompilerTests()
        {
            //
            // TODO: Add constructor logic here
            //
        }

        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        //
        // You can use the following additional attributes as you write your tests:
        //
        // Use ClassInitialize to run code before running the first test in the class
        // [ClassInitialize()]
        // public static void MyClassInitialize(TestContext testContext) { }
        //
        // Use ClassCleanup to run code after all tests in a class have run
        // [ClassCleanup()]
        // public static void MyClassCleanup() { }
        //
        // Use TestInitialize to run code before running each test 
        // [TestInitialize()]
        // public void MyTestInitialize() { }
        //
        // Use TestCleanup to run code after each test has run
        // [TestCleanup()]
        // public void MyTestCleanup() { }
        //
        #endregion

        [TestMethod]
        public void CompilerEmitterPushIntTest()
        {
            HvmEmitter emitter = new HvmEmitter(5);
            emitter.push(10);
            CollectionAssert.AreEqual(emitter.complete(), new byte[] { 13, 0, 0, 0, 10 });
        }

        [TestMethod]
        public void CompilerEmitterPushStringTest() 
        {
            HvmEmitter emitter = new HvmEmitter(12); // 3 for opcode, and size
            string test = "this is 9";
            ;
            emitter.push(test);
            CollectionAssert.AreEqual(emitter.complete(), new byte[] { 77, 0, 9 }.Concat(test.ToCharArray().Select(a => (byte)a).ToArray()).ToArray());
        }
    }
}
