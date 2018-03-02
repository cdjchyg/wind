using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MonoCsharp
{
    public static class MainTest
    {
        static int mVar = 1;

        static void Main()
        {
			try{
            System.Console.WriteLine("你好,世界 " + mVar.ToString());

            //mVar = 1/mVar;

            //++mVar;
            throw new ArgumentException("test");
			}
			catch(ArgumentException e){
				Console.WriteLine(e.ToString());
			}
        }
    }
}