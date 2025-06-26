# Objective-Bucket
The Objective Bucket is a game-tracking device developed for Derby Darts nerf club. It is designed to enable some of the clockwork of fun gamemaking, including timers, counters, scoring and anything else that is better handled by a computer.

## Hardware
Electronically, the Objective Bucket uses 5 switchs and a 20x4 LCD display.
4 of the buttons serve as the primary game inputs, with 1 used to return to the admin menu. 
The Display is used for all gameplay feedback.

## Wishlist
Features to add:
- Progress bars: https://github.com/Gjorgjevikj/SmoothProgress.
- Double/Triple/Quad-line-sized text.
- Extensible IO hardware for RFID, keys, mag-sense, shake-sense, etc.
- RGB LED strip for high visibility feedback.

Gamemodes to add:
- TDM (buttons count deaths per team)
- KotH Timer - Each button has a timer. When button X is pressed, all other timers pause and timer X resumes. reset = all timers to 0:00.
- Life counters - When button X is pressed, counter X increments (For now, we can just tell players, "if your count equals 3, you die", for example.)
- Fifth Element - When all buttons are held, a 10 second timer is started. If any button is released, the timer resets. If the timer completes, the screen flashes on and off.
