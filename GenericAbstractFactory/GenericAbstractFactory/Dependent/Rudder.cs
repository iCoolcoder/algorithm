using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;

namespace GenericAbstractFactory.Dependent
{
    class Rudder : IProduct<Plane>
    {
        public void Operate()
        {
            Console.WriteLine("Operating Rudder.");
        }

        public void RudderSpecificOperation()
        {
            Console.WriteLine("Performing Rudder-specific operation.");
        }
    }
}
