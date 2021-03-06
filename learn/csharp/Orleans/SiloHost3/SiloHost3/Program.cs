using System;

using Orleans;
using Orleans.Runtime.Configuration;
using Orleans.Runtime.Host;

// Orleans download:
// https://marketplace.visualstudio.com/items?itemName=sbykov.MicrosoftOrleansToolsforVisualStudio

namespace SiloHost3
{
    /// <summary>
    /// Orleans test silo host
    /// </summary>
    public class Program
    {
        static void Main(string[] args)
        {
            // First, configure and start a local silo
            var siloConfig = ClusterConfiguration.LocalhostPrimarySilo();
            var silo = new SiloHost("TestSilo", siloConfig);
            silo.InitializeOrleansSilo();
            silo.StartOrleansSilo();

            Console.WriteLine("Silo started.");

            //// Then configure and connect a client.
            //var clientConfig = ClientConfiguration.LocalhostSilo();
            //var client = new ClientBuilder().UseConfiguration(clientConfig).Build();
            //client.Connect().Wait();

            Console.WriteLine("Client connected.");

            //
            // This is the place for your test code.
            //
            //var friend = client.GetGrain<GrainInterfaces3.IGrain3>(0);
            //Console.WriteLine("\n\n{0}\n\n", friend.SayHello().Result);

            Console.WriteLine("\nPress Enter to terminate...");
            Console.ReadLine();

            //// Shut down
            //client.Close();
            silo.ShutdownOrleansSilo();
        }
    }
}
