using System;
using System.Collections.Generic;
using System.Text;

namespace GenericAbstractFactory.Common
{
    class Plane : IFactory<Plane>
    {
        public TProduct Build<TProduct>() where TProduct : IProduct<Plane>, new()
        {
            Console.WriteLine("Creating Plane: " + typeof(TProduct));
            return new TProduct();
        }
    }
}
