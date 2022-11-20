# Physics-Pinball
This Pinball is based on the Mario Kart course Waluigi Pinball, the sprites are handmade and inspired from the elements of that game.

[GitHub link](https://github.com/PauFusco/Physics-Pinball)
# Controls
- DOWN  Arrow: Launch Ball
- LEFT  Arrow: Left  Flipper
- RIGHT Arrow: Right Flipper

### Debug
  - F1: Toggle colliders and ball drag with left click
  - F2: Toggle Background rendering
  - F10: Increase Bumper strength
  - F11: Decrease Bumper strength

# Autors
This minigame was made by:
  - Hector Garcia - Art and Sound
  - Kaede Sato - Code
  - Dani Garcia - Code
  - Pau Fusco - Code

# Development issues
While developing the project we run into some problems regarding interactions between the ball and some other objects.

For example, in the case of the bumpers we did not know how to program their function in order to make the ball bounce but without making the coding too difficult. Our first idea was to use the angle of the tangent of the bumper when the ball hit it, but we felt it was too complicated. In order to work around it, we thouhgt of using the vector made by the center of the bumper and the ball, making it the vector which would make the ball bounce. So we take the position of both bodies, take the vector that they make and use it as a linear impulse. We ended up making the bigger bumpers make the ball bounce more than the other ones, since the distance from the point of impact and its center is bigger than with the small ones, making them have a bigger vector of impulse.

In order to make the ball rotate without its friction being too big we just made the friction of the walls small enough to not slow the ball to much, but big enough so that it rotated. At the start we used a quoficient of 0, but the movement didnt feel as satisfing, that's why we changed it.

We found some design issues with the flippers since the map we designed at the start had them oriented with a positive angle. This made the ball get stuck in the point of rotation, since when they were not active, the flippers served as a ramp and made the ball go down and stop at a point that activating the flippers did not move it. In the end we made the flippers stay at a positive angle, making it impossible for the ball to get stuck there.
