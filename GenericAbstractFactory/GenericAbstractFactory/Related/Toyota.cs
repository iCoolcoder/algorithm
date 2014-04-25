using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;

namespace GenericAbstractFactory.Related
{
    class Toyota : IProduct<Car>
    {
        public void Operate()
        {
            Console.WriteLine("Driving Toyota.");
        }

        public void ToyotaSpecificOperation()
        {
            Console.WriteLine("Performing Toyota-specific operation.");
        }
    }
}
