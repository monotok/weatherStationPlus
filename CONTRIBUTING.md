#Contributing Guide

I hope you find this software useful and contributions are always welcome. Just a few
usual things to try and follow to keep things tidy.

I use my own gitlab instance with my own CI pipelines setup and any changes
get pushed to github.

## Pull Requests

- Try to keep the commit messages specific and combined into one. I use the Jetbrains
IDE called Clion which makes it easier.
- Quick summary of what you are changing to make it easier to review.
- I'll run the code against my CI pipeline.

## Coding Style

A few points to coding style.

- Functions and classes etc normally have the open bracket underneath. I know most IDEs
default to the same line but I find it easier to read when it is underneath.
- Most places I have put the '*' on the type rather than the variable name.

## Continuous Integration

Like I mentioned above I have a CI pipeline which builds the code using docker and then
runs the unit tests onto the raspberry pi. I also deploy the main codebase to check everything
is working as expected.

## Testing

I have tried to be good and written unit tests with a coverage report. The pipeline does 
display the coverage percentage. New code shouldn't make the test coverage go down so any new 
functionality should have tests written for it.

# Thanks!

Enjoy!