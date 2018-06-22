using System;

using Orleans;
using Orleans.Runtime.Configuration;
using Orleans.Runtime.Host;
using EmployeeGrainInterfaces;

namespace EmployeeClient
{
    /// <summary>
    /// Orleans test silo host
    /// </summary>
    public class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Waiting for Orleans Silo to start. Press Enter to proceed...");
            Console.ReadKey();

            var config = Orleans.Runtime.Configuration.ClientConfiguration.LocalhostSilo(40000);
            GrainClient.Initialize(config);
            Console.WriteLine("Client connected.");

            var grainFactory = GrainClient.GrainFactory;
            //var e0 = grainFactory.GetGrain<IEmployee>(Guid.NewGuid());
            //var e1 = grainFactory.GetGrain<IEmployee>(Guid.NewGuid());
            //var e2 = grainFactory.GetGrain<IEmployee>(Guid.NewGuid());
            //var e3 = grainFactory.GetGrain<IEmployee>(Guid.NewGuid());
            //var e4 = grainFactory.GetGrain<IEmployee>(Guid.NewGuid());

            //var m0 = grainFactory.GetGrain<IManager>(Guid.NewGuid());
            //var m1 = grainFactory.GetGrain<IManager>(Guid.NewGuid());
            //var m0e = m0.AsEmployee().Result;
            //var m1e = m1.AsEmployee().Result;

            //m0e.Promote(10);
            //m1e.Promote(11);

            ////m0.AddDirectReport(e0).Wait();
            ////m0.AddDirectReport(e1).Wait();
            ////m0.AddDirectReport(e2).Wait();
            ////m1.AddDirectReport(m0e).Wait();
            ////m1.AddDirectReport(e3).Wait();
            ////m1.AddDirectReport(e4).Wait();

            //m0.AddDirectReport(e0);
            //m0.AddDirectReport(e1);
            //m0.AddDirectReport(e2);
            //m1.AddDirectReport(m0e);
            //m1.AddDirectReport(e3);
            //m1.AddDirectReport(e4);

            var e0 = GrainClient.GrainFactory.GetGrain<IEmployee>(Guid.NewGuid());
            var m1 = GrainClient.GrainFactory.GetGrain<IManager>(Guid.NewGuid());
            m1.AddDirectReport(e0).Wait();

            Console.WriteLine("\nPress Enter to terminate...");
            Console.ReadKey();
            // Shut down
            GrainClient.Uninitialize();
        }
    }
}
