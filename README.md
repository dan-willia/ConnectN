# ConnectN

ConnectN is desktop Qt application that extends the classic Connect-4 game to Connect-N, where N is between 4 and 10. Board is dynamically generated depending on size of N. Players have special abilities like erasing rows and converting disks, and a 7-second timer that runs during each turn.

## Game features
- file storage
- dynamic board generation
- Qt animations
- QTimer
- custom signal/slots
- custom QObject
- MVC design

Have a look at the [video demo](https://youtu.be/oc9h_X-DQfA) to see the game features in action.

## MVC architecture
The `ConnectN`, `Board`, and `Player` classes work together to form the underlying data model for the game. These use standard C++ data types like `int`, `string`, and `vector`.

![ConnectNClassDiagram](/assets/ConnectNClassDiagram.drawio.svg)

##### *UML diagram showing the relationship between the main classes*

A new game can be initialized by passing three parameters to the constructor: n, player 1's name, and player 2's name. `ConnectN` then creates the Board and Player instances.

```C++
#include "connectn.h"
#include <iostream>

int main()
{
  ConnectN game{4, "Alice", "Bob"};
  std::string game_string{game.Stringify()};
  std::cout << game_string << std::endl;
  // outputs a csv representation
}
```

QWidgets function as the view and controllers for the application, with `ConnectNWindow` as the main coordinating window.

### Animations
The main UI component in the app are the disks. I implemented a custom `QObject`-derived class `Disk` to create the animations. The disks have a bounding rectangle with an interior circle:

![Disk example](/assets/disk.png)

##### *Disks drawn with the (usually hidden) bounding rectangle*

The disks emit a `mousePressed` signal to the main window, which then dispatches the appropriate handler based on the mode. For example, if the user has clicked "convert disk", this puts the game into "convert disk mode", and the disk will change to the player's color.

### Reading/Writing Data
Having a simple data model for the game was essential to easily saving and loading games. All that is required is to `Stringify` the model and then write it to the appropriate location. I learned that operating systems have a folder where applications can store persistent data, and that Qt can automatically detect it based on your OS (cross-platform compatibility!). 

To load games, I created a factory class that creates a new game instance from a collection of string arguments, which corresponds to the output of the `Stringify()` method in ConnectN.

```C++
connectnfactory::CreateGame(std::vector<std::string> args)
```

## Challenges
### Really understanding OOP
If there's one big design lesson I learned from the project, it's that it really pays to separate the data model from the view. Originally, there were no separate Board or Player classes, and `ConnectN` stored the `QObject` `Disks` as a data member. This made managing the board configuration quite messy when I came around to animating the disks. So halfway through the project I decided to redesign the ConnectN class to have only basic data types. I then created new classes for the board and players. Even though this was our third assignment using OOP, it wasn't until this one that I really felt the benefits of having a cleanly delineated class structure. It was much easier to animate disks now that the configuration could just be read off the model; the view class would only be concerned with rendering it and some additional animations. Cool. 

### Asynchronous programming
I took this class after Systems and at the same time as Operating Systems, so I understood signals and had some practice programming with them. But I felt like I levelled up my understanding with it on this project. For example, the `animation_ip_` flag started out as an animation tracker but evolved into a general synchronization mechanism - essentially acting as a mutex to coordinate user input, animations, and timer events.

## Next steps

### Ui
- Larger and more eye-catching indication of whose turn it is
- More attractive color scheme for special ability buttons
- Show what "mode" a player is in, e.g. by highlighting the selected ability

### Backend
- Replace the all-purpose `animation_ip_` flag with a better named or separate flags
- `ConnectN` interface is missing a method that checks if the game is over; it only calculates the number of rounds won by each player. 

But overall it's a completely functioning application as is.

--- 

Interested in the source code? Feel free to reach out - I'm happy to discuss the implementation in detail. 
