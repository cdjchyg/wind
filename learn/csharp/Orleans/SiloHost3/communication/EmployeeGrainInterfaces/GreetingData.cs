using Orleans.Concurrency;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EmployeeGrainInterfaces
{
    /*
         Messages are simply data passed from one actor to another, we just created the GreetingData class to do just this.

    In .NET, most objects are created from a class of some sort and are passed around by reference, something that doesn't 
    work well with concurrency, and definitely not with distribution.

    When Orleans sends a message from one grain to another, it creates a deep copy of the object, and provides the copy to 
    the second grain, and not the object stored in the first grain. This prohibits the mutation of state from one grain to 
    another, one of the main tenets in the actor model is that state shouldn't be shared, and message passing is the only 
    mechanism for exchanging data.

    When the grains are in different silos, the object model is serialized to a binary format, and sent over the wire.

    However, this deep copy process is expensive, and if you promise not to modify the message, then for communication with 
    grains within a silo, it's unnecessary.

    If you indicate to Orleans that you are not going to modify the object (i.e. it's immutable) then it can skip the deep 
    copy step, and it will pass the object by reference. There's no way Orleans or C# can stop you from modifying the state, 
    you have to be disciplined.

    Immutability is indicated with a the [Immutable] attribute on the class:

    [Immutable]
    public class GreetingData
    {
        public Guid From { get; set; }
        public string Message { get; set; }
        public int Count { get; set; }
    }
     */
    [Immutable]
    public class GreetingData
    {
        public Guid From { get; set; }
        public string Message { get; set; }
        public int Count { get; set; }
    }
}
