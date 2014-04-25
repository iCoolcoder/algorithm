using System;
using System.Collections.Generic;
using System.Text;

using GenericAbstractFactory.Common;
using GenericAbstractFactory.Dependent;
using GenericAbstractFactory.Related;

namespace GenericAbstractFactory
{
    class Program
    {
        public static void Main()
        {
            Program program = new Program();
            program.DependentCalls();
            program.RelatedCalls();
        }

        public void DependentCalls()
        {
            Console.WriteLine("##DEPENDENT CALLS:" + Environment.NewLine);
            
            Console.WriteLine("*Car Factory:");
            Factory<Car> carFactory = new Factory<Car>();
    
            // Example of using interface functions.
            IProduct<Car> carProduct = carFactory.Create<Engine<Car>>();
            carProduct.Operate();
            //carProduct.EngineSpecificOperation(); // Not in interface.
    
            //IProduct<Plane> planeProduct = carFactory.Create<Engine<Car>>(); // Cannot implicitly convert type "Engine<Car>" to "IProduct<Plane>".
            //Frame<Car> carFrame = carFactory.Create<Engine<Car>>(); // Cannot implicitly convert "Engine<Car>" to "Frame<Car>".
    
            // Example of using a product specifc function.
            Frame<Car> carFrame = carFactory.Create<Frame<Car>>();
            carFrame.Operate();
            carFrame.FrameSpecificOperation();
            Console.WriteLine();
    
            // Good news:  This is now a compile time rather than runtime error.
            //carFactory.Create<Rudder>();        // "Rudder" must be convertible to "IProduct<Car>".
            //carFactory.Create<Engine<Plane>>(); // "Engine<Plane>" must be convertible to "IProduct<Car>".
    
            Console.WriteLine("*Plane Factory:");
            Factory<Plane> planeFactory = new Factory<Plane>();
            //IProduct<Car> bad = planeFactory.Create<Rudder>(); // Cannot implicitly convert type "Rudder" to "IProduct<Car>".
            IProduct<Plane> rudderProduct = planeFactory.Create<Rudder>();
            rudderProduct.Operate();
            Console.WriteLine();
            
            // Cannot create engine of the other factory.
            //carFactory.Create<Engine<Plane>>(); // "Engine<Plane>" must be convertible to "IProduct<Car>".
            //planeFactory.Create<Engine<Car>>();   // "Engine<Car>" must be convertible to "IProduct<Plane>".
    
            // Example of a car factory creating a plane frame.
            carFactory.Create<Frame<Plane>>();
    
            // Example of a plane factory creating a car frame.
            planeFactory.Create<Frame<Car>>();

            Console.WriteLine();
        }

        private void RelatedCalls()
        {
            Console.WriteLine("##RELATED CALLS:" + Environment.NewLine);

            Console.WriteLine("*Car Factory:");
            Factory<Car> carFactory = new Factory<Car>();

            // Example of using interface functions.
            IProduct<Car> carProduct = carFactory.Create<Toyota>();
            carProduct.Operate();
            //carProduct.ToyotaSpecificOperation(); // Not in interface.

            //IProduct<Plane> planeProduct = carFactory.Create<Honda>(); // Cannot implicitly convert "Honda" to "IProduct<Plane>".
            //Toyota toyota = carFactory.Create<Honda>(); // Cannot implicitly convert "Honda" to "Toyota".

            // Example of using a product specific function.
            Honda honda = carFactory.Create<Honda>();
            honda.Operate();
            honda.HondaSpecificOperation();
            Console.WriteLine();

            Console.WriteLine("*Plane Factory:");
            Factory<Plane> planeFactory = new Factory<Plane>();
            //IProduct<Car> bad = planeFactory.Create<Boeing>(); // Cannot implicitly convert type "Boeing" to "ICarProduct".
            IProduct<Plane> boeing = planeFactory.Create<Boeing>();
            boeing.Operate();
            Console.WriteLine();

            Console.WriteLine("*Car and Plane Factory:");
            //carFactory.Create<Boeing>(); // "Boeing" must be convertible to "IProduct<Car>".
            //planeFactory.Create<Honda>(); // "Honda" must be convertible to "IProduct<Plane>".
            //planeFactory.Create<Toyota>(); // "Toyota" must be convertible to "IProduct<Plane>".

            Saab saab1 = carFactory.Create<Saab>();
            saab1.Operate();
            saab1.SaabSpecificOperation();

            Saab saab2 = planeFactory.Create<Saab>();
            saab2.Operate();
            saab2.SaabSpecificOperation();

            Console.WriteLine();
        }
    }
}
