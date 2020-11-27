package section;

@license(LGPL);
//
// TODO documentation
//
network test
{
    @display("bgb=282,237");
    submodules:
        tic: Node {
            @display("p=58,113");
        }
        toc: Node {
            @display("p=211,113");
        }
    connections:
        tic.out --> toc.in;
        toc.out --> tic.in;
}

