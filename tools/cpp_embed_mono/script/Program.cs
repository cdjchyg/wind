using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace MonoCsharp
{
    public static class MainTest
    {
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern static string gimme();
		
        static int mVar = 1;

        static void Main()
        {
            System.Console.WriteLine("你好,世界 " + mVar.ToString() + " " + gimme() );

            //mVar = 1/mVar;

            //++mVar;
            
        }
    }
}