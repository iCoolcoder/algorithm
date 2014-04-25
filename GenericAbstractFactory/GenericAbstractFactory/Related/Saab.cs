using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;

namespace GenericAbstractFactory.Related
{
    class Saab : IProduct<Car>, IProduct<Plane>
    {
        public void Operate()
        {
            Console.WriteLine("Operating Saab.");
        }

        public void SaabSpecificOperation()
        {
            Console.WriteLine("Performing Saab-specific operation.");
        }
    }
}
