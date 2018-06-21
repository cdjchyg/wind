using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Threading;

namespace async_await
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("step1.");
            func();
            Console.WriteLine("step2.");

            Thread.Sleep(1000);


            /*
             * async 表示func为异步方法
             * func执行到await时，立即返回
             * 当await操作完成时，继续执行func后面的逻辑
             */
        }

        static async void func()
        {
            int i = 50;
            await Task.Run( ()=>
            { 
                Thread.Sleep(500);
                Console.WriteLine("Thread end!"); 
            } );
            Console.WriteLine("asyncFunc end! " + i.ToString());
        }
    }
}
