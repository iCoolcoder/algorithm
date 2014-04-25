using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;

namespace GenericAbstractFactory.Related
{
    class Honda : IProduct<Car>
    {
        public void Operate()
        {
            Console.WriteLine("Driving Honda.");
        }

        public void HondaSpecificOperation()
        {
            Console.WriteLine("Performing Honda-specific operation.");
        }
    }
}
