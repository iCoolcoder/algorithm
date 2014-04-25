using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;

namespace GenericAbstractFactory.Related
{
    class Boeing : IProduct<Plane>
    {
        public void Operate()
        {
            Console.WriteLine("Flying Boeing.");
        }

        public void BoeingSpecificOperation()
        {
            Console.WriteLine("Performing Boeing-specific operation.");
        }
    }
}
