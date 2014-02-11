using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace FileRevTest
{
    class Program
    {
        static void Main(string[] args)
        {
            Fun.FileReverse("C:\\dir.txt", new Int16[1]{ 0 });
        }
    }
}
