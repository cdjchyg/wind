using System;
using System.Threading.Tasks;
using Orleans;
using System.Threading;

//http://dotnet.github.io/orleans/1.5/Tutorials/Interaction-with-Libraries-and-Services.html

namespace Stock
{
    public class StockGrain : Grain, IStockGrain
    {
        string mPrice;
        string mData;
        public override async Task OnActivateAsync()
        {
            string stock;
            this.GetPrimaryKey(out stock);
            await UpdatePrice(stock);
            RegisterTimer(
                UpdatePrice,
                stock,
                TimeSpan.FromSeconds(10),
                TimeSpan.FromSeconds(10)
                );
            await base.OnActivateAsync();
        }

        async Task UpdatePrice(Object stock)
        {
            Console.WriteLine("UpdatePrice begin:" + DateTime.Now.ToString("hh:mm:ss"));

            /*
             However, by doing this we're waiting for the price from Yahoo, 
             and after that's complete we request the graph data. This is inefficient, 
             as we could be doing these at the same time. Fortunately, Task has a 
             convenient WhenAll method which allows us to await multiple tasks at once, 
             allowing these tasks to complete in parallel
             */
            //mPrice = await GetPriceFromYaoo(stock as string);
            //mData = await GetGraphData(stock as string);//await过后,返回的结果已经是Task unwrapper的结果

            var data = GetGraphData(stock as string);
            var price = GetPriceFromYaoo(stock as string);
            await Task.WhenAll(price, data);
            mPrice = price.Result;
            mData = data.Result;

            Console.WriteLine("Price:" + mPrice.ToString() + " " + mData + " " + DateTime.Now.ToString("hh:mm:ss"));
        }

        async Task<string> GetGraphData(string stock)
        //Task<string> GetGraphData(string stock)
        {
            Console.WriteLine("GetGraphData Begin");
            //Thread.Sleep(3000);
            //return Task.FromResult("graph data");

            await Task.Factory.StartNew(() =>
            {
                Thread.Sleep(3000);
            });

            return "graph data";
        }

        async Task<string> GetPriceFromYaoo(string stock)
        {
            Console.WriteLine("GetPriceFromYaoo Begin");
            var res = await Task.Factory.StartNew( ()=> {
                Thread.Sleep(3000);
                return "50$";
                //return Task.FromResult("50$");
            } );
            //return res.Result;
            return res;
        }

        public Task<string> GetPrice()
        {
            return Task.FromResult(mPrice);
        }
    }
}
