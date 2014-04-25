using System;
using System.Collections.Generic;
using System.Text;

namespace GenericAbstractFactory.Common
{
    interface IFactory<TFactory>
    {
        TProduct Build<TProduct>() where TProduct : IProduct<TFactory>, new();
    }
}
