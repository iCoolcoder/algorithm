using System;
using System.Collections.Generic;
using System.Text;

namespace GenericAbstractFactory.Common
{
    interface IProduct<TFactory>
    {
        void Operate();
    }
}
