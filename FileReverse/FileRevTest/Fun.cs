using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace FileRevTest
{
    class Fun
    {
        [DllImport("FileReverse", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern System.Int32 FileReverse(System.String pathName, Int16[] pbIsTextUnicode);
    }
}
