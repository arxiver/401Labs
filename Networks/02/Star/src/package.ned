package star;

@license(LGPL);

import ned.IdealChannel;

network Star
{
    parameters:
        int n = default(5);
    submodules:
        nodes[n]: Node ;
        hub : Hub;
    connections: 
       for i=0..sizeof(nodes)-1
       {
           nodes[i].out --> IdealChannel --> hub.ins++;
           nodes[i].in <-- IdealChannel <-- hub.outs++;
       }  
}
