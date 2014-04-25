using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;

namespace GenericAbstractFactory.Dependent
{
    class Frame<TFactory> : IProduct<Car>, IProduct<Plane>
    {
        public void Operate()
        {
            Console.WriteLine("Operating Frame.");
        }

        public void FrameSpecificOperation()
        {
            Console.WriteLine("Performing Frame-specific operation.");
        }
    }
}
