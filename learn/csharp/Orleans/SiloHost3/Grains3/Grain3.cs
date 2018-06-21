using System.Threading.Tasks;
using Orleans;

namespace Grains3
{
    /// <summary>
    /// Grain implementation class Grain3.
    /// </summary>
    public class Grain3 : Grain, GrainInterfaces3.IGrain3
    {
        private string mText = "I'm From Grain3!";
        public Task<string> SayHello(string greeting)
        {
            var oldText = mText;
            mText = greeting;
            return Task.FromResult(oldText);
        }
    }
}
