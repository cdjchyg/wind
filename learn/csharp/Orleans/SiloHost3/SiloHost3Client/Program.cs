using System;

using Orleans;
using Orleans.Runtime.Configuration;
using Orleans.Runtime.Host;

namespace SiloHost3Client
{
    /// <summary>
    /// Orleans test silo host
    /// </summary>
    public class Program
    {
        static void Main(string[] args)
        {
            //// First, configure and start a local silo
            //var siloConfig = ClusterConfiguration.LocalhostPrimarySilo();
            //var silo = new SiloHost("TestSilo", siloConfig);
            //silo.InitializeOrleansSilo();
            //silo.StartOrleansSilo();

            //Console.WriteLine("Silo started.");

            Console.WriteLine("Waiting for Orleans Silo to start. Press Enter to proceed...");
            Console.ReadLine();

            // Then configure and connect a client.
            var clientConfig = ClientConfiguration.LocalhostSilo();
            var client = new ClientBuilder().UseConfiguration(clientConfig).Build();
            client.Connect().Wait();

            Console.WriteLine("Client connected.");

            //
            // This is the place for your test code.
            //
            var friend = client.GetGrain<GrainInterfaces3.IGrain3>(0);

            Console.WriteLine("\n\n");
            Console.WriteLine(friend.SayHello("Frist").Result);
            Console.WriteLine(friend.SayHello("Second").Result);
            Console.WriteLine(friend.SayHello("Third").Result);
            Console.WriteLine("\n\n");
            Console.WriteLine("\nPress Enter to terminate...");
            Console.ReadLine();

            // Shut down
            client.Close();
            //silo.ShutdownOrleansSilo();
        }
    }
}
