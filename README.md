# NEETE
![NETE logo](/images/nete_logo.png)
## Introduction
Computer programs are normally written as plain text documents, usually as a long trail of lines from top to bottom. I think it would be a nice touch if an artistic dimension could be added to that code to enhance meaning.

NEETE, meaning Next Evolution Enhanced Text Editor, is a way in which the programmer can embed a diagrammatic overview to the code and thus summarise the underlying operations better. It uses a graphical approach to creating and editing text. Graphical here is capturing two senses of the word i.e. 

1. Drawing pictures on a screen,
2. Mathematical nodes and arcs.

The entire proposition is carried by the two elements i.e. node and arc, and how these are creatively combined to produce the intended output in a project.
## How to Run it
So far this progam has been tested on Linux x86 and Raspberry Pi. The code requires GTK3, libgtksourceview and some other few packages to compile. I acquired them with the command:
````
sudo apt-get install build-essential libgtk-3-dev libgtksourceview-3.0-dev
````
Then compile the source by running:
````
make
````
Then the program can be run with the command:
````
./nete_exec
````
## Quick Start
When the program starts it has a very simple interface which has a toolbar and a canvas.

![Interface on start](/images/typicalinterface.png)

The toolbar has an entry box for finding line numbers and three radio buttons for the editing mode, which are: ``Move``, ``Contain`` and ``Link``.

* ``Move`` allows the nodes to be moved and resized. Moving is done by dragging the name tag. Resizing is achieved by dragging the square shaped resize grip on the bottom right corner.
* ``Contain`` allows nodes to be put into containers. This is done by clicking the name tag of the node to move. At this point the name tag should take on a blue background. Then the next step is to select a point in the body of the destination node.
* ``Link`` will connect to nodes with an arc. This is done by clicking in the body of the first node. The name tag will be highlighted in blue, then click in the body of the node to follow. An arc should appear linking the two. Removing an  arc is the same process on already connected nodes.

![A selected node](/images/nodehighlight.png)

Double clicking the name tag opens a window in which details of that node can be edited.

New nodes can be created by right clicking at a desired location in the canvas and selecting **New Node**. Nodes created in the body of another node are automatically contained by that node.

Nodes can be copied and pasted simply by right clicking on the name tag and selecting **Copy**, then right clicking on an intended destination and selecting **Paste**. A node is copied along with all the other contents it carries.

By default, output text is written to the file **nete_out.txt**. This can be changed under **Edit** then **Option** on the top menu. A window with an entry box will appear where a complete path to a desired output file can be entered.

Whenever the text represented by NEETE is required, the output file can be rendered under **File** then **Render** or by pressing **CTRL+SHIFT+R**.
## How it Works
## Nodes
The nodes carry the bulk of the work in making all the magic happen. The same node is used in two scenarios:


1. To carry text (where it's termed a codebox),
2. To carry nodes (where it's termed a container).


The same one node does both jobs but in different scenarios. The determinant is whether or not it contains another node and this difference shows up during rendering.

Nodes also carry with them two other visible features, which are title and priority. A description field is included and this is more or less a comment feature with no real consequence on operations.
### Codebox - Head, Tail and Textbox
Codebox structure             |  Real CodeBox
:-------------------------:|:-------------------------:
![Codebox Structure](/images/codebox_struct3.png) | ![Codebox Example](/images/codeboxwithcode3.png)

On the inside of a node there are three other important text fields, which are the head, tail and textbox. These are pertinent for the actual low level of operations and they form the basis of the text. Upon rendering a codebox, these elements are batched in the order:


1. Head,
2. Textbox,
3. Tail.


The head and tail are very important for containers because they allow an operator to be bound to the contained nodes.

The head and tail can also be useful in helping maintain codebox scope, because they allow mandatory fields to be taken out of the core code at hand and be maintained separately. An example could be a ``for`` statement where the head would contain 
````javascript 
		for(i = 0; i < 10; i++) { 
  ```` 
  and the tail may have ``}`` then the body is in the textbox. That way the programmer can focus their attention better on the body without being distracted by the syntax of the ``for``.
  
The textbox is meant to carry small manageable snippets of code. This is where the bulk of the text is carried in the project. The size of this snippet is up to the discretion of the programmer. The idea here being that it must be a simple enough section to read through and understand in a short space of time.
### Container
In the case of a container, every aspect of the codebox applies, except that the textbox is ignored. This is because the role of the textbox is taken by the contained nodes and these contained nodes are rendered recursively to eventually produce the contained text.

Containers may act as passive holders of nodes purely for logical grouping or they can apply an operator to those nodes that they contain. An example of such could be a ``for`` loop, a ``while`` operator, an ``if`` statement, e.t.c. This comes about because the head and tail give containers the ability to hold scope over the nodes that they contain.
## Arcs
![Nodes joined by arcs](/images/nodesfollowing2.png)

The arc is present to allow a follow sequence to be determined and it outputs from the right of a node and inputs into the left of the follower.
## Putting it Together

![Nodes of different priorities in a container](/images/nodesandcontainer2.png)

The relationship between codeboxes, containers and arcs then comes together during rendering. Here a few simple rules determine the outcome. Text in the output comes from three sources, which are:


1. Heads,
2. Textboxes,
3. Tails.


When the system renders a node, it begins with the head. Then depending on whether it's a codebox or a container it will render the textbox or the contained nodes recursively and lastly renders the tail.
In order to render a node it has to pick the node and have a reason to move to the next node. Everything starts from the first node. The first node is found at the highest level of nodes i.e. nodes which are not contained by any other nodes. Nodes at any equal level in a container are then ordered by priority and ordered by followings (i.e. arcs).

![Nodes set to show the colour coded priorities](/images/colour_codes2.png)

The nodes may have 10 priorities as shown in the figure, so this feature must be used sparingly. Priority 0 is the highest and 9 is the lowest. With nodes at the same level the highest priority is rendered first and the lowest is rendered last. The priority is visible as a colour code on the frame of the node. Nodes which have the same priority, at the same level of a container can be considered to be picked in random order(useful for parallel thinking).

Arc following is the other rule which determines ordering or rendering. Here the system will follow the arcs in its path.
Using this simple set of rules, the system will stitch together all the text contained in the nodes and give out a rendered text file to be used as desired. 
## Focus and Ignore
### Focus
This is a function that is used on containers to have elements contained in a selected node being the only visible ones on the screen, thus putting them in focus. It is useful whenever attention needs to be placed on a particular container. 


Different Sizes             |  Focus Node
:-------------------------:|:-------------------------:
![Different Size Nodes](/images/three_size_node2.png) | ![Node in Focus](/images/focus_node2.png)




A situation though where "Focus" is imperative is a case where a container has been shrunk to a point where the nodes contained become hard to see or where NEETE has determined that they are too small to render. At that point, the inside of the container will have a hatched pattern as in the left figure  above. This pattern reminds the user that the node is a container and has elements which have become too small to see. In order to view details on such a container either it have to be expanded or it will need to be focused. When in focus mode, the top bar changes to highlight the name of the container as in right side figure above.

This feature allows the user to drill into deeper and deeper detail as and where necessary allowing control over the amount of attention needed in different scenarios. 
### Ignore
"Ignore" is a tick box that can be selected on any node, be it a container or a codebox. Once selected, the node will be struck out visually with an "X" as below. 

![Node to ignore](/images/ignore_node.png)

A node which is set to "Ignore" will have its contents ignored on the text output rendering. Therefore it serves the same purpose as commenting out lines of source code.
## Example
This README is a typical example of NEETE in real life operation. The overview is shown in the figure below and the _.nete_ file is available in the **examples** folder.

![README.md NEETE overview](/images/neteexample4.png)
## Acknowledgements
This project uses the following libraries in its operations:

* **cJSON** by Dave Gamble, from https://github.com/DaveGamble/cJSON
* **dynarray** from https://github.com/eignnx/dynarray

