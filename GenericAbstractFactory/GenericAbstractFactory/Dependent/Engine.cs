using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;

namespace GenericAbstractFactory.Dependent
{
    class Engine<TFactory> : IProduct<TFactory>
    {
        public void Operate()
        {
            Console.WriteLine("Operating Engine.");
        }

        public void EngineSpecificOperation()
        {
            Console.WriteLine("Performing Engine-specific operation.");
        }
    }
}
