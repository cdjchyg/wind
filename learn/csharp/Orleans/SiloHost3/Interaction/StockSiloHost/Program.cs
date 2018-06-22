using System;

using Orleans;
using Orleans.Runtime.Configuration;
using Orleans.Runtime.Host;

namespace Stock
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

            // Then configure and connect a client.
            var clientConfig = ClientConfiguration.LocalhostSilo();
            var client = new ClientBuilder().UseConfiguration(clientConfig).Build();
            client.Connect().Wait();

            Console.WriteLine("Client connected.");

            //
            // This is the place for your test code.
            //
            Console.WriteLine("Step1");
            var grain = client.GetGrain<IStockGrain>("MSFT");
            Console.WriteLine("Step2");
            var price = grain.GetPrice().Result;
            Console.WriteLine("Step3");
            Console.WriteLine("P:" + price.ToString());

            //������:����� Step1 step2����10s�������Step3
            //��ΪStockGrain��OnActivateAsync���첽����������GetGrain����ٷ��ء�
            //StockGrainֻ�д�����OnActivateAsync�������󣬲Żᴦ��GetPrice.

            Console.WriteLine("\nPress Enter to terminate...");
            Console.ReadLine();

            // Shut down
            client.Close();
            silo.ShutdownOrleansSilo();
        }
    }
}
