using System;
using System.Collections.Generic;
using System.Text;

namespace GenericAbstractFactory.Common
{
    class Factory<TFactory> where TFactory : IFactory<TFactory>, new()
    {
        public TProduct Create<TProduct>() where TProduct : IProduct<TFactory>, new()
        {
            return new TFactory().Build<TProduct>();
        }
    }
}
