# About the project
A game prototype that explores and recreates the Leviathan Axe throw mechanic from the Nordic God of War series using Unreal Engine 5. 

[![image_alt](https://github.com/srserge/God-Of-War-Axe/blob/5e91d28089713084189b46f5277e29d92a208868/god-of-war-axe-throw-cover.png)](https://youtu.be/UR2G9bh5-0k?si=doCtxTWKJcazKAkK)
_Click on this image to watch the video showcasing the playable demo._

> This is a study project based on the original **God of War** (2018) IP, developed by Santa Monica Studio.

## Table of Contents ##
- [Technologies](#technologies)
- [Features](#features)
- [Development Process](#development-process)
- [Game Controls](#game-controls)
- [Learnings](#learnings)
- [Future Improvements](#future-improvements)
- [Acknowledgments](#acknowledgments)
- [How to Run the Project](#how-to-run-the-project)

## Technologies ##
- <ins> C++ </ins>
- <ins> Unreal Engine 5.3.2 </ins>
- <ins> JetBrains Rider IDE </ins>

## Features ##
- Axe throw
- Axe recall
- Destroy interactive objects
- Deal damage to enemies
- Embed the axe in the environment geometry.

## Development Process ##
- I started by defining the folder hierarchy and classes before writing a single line of code, which allowed the structure to adapt smoothly as the project moved forward.

- I estimated a two-week timeline; some implementation challenges extended this time, though overall it was completed within the planned deadline. Using Notion for task management (Kanban methodology) helped me to keep things organized during the project.

- Next, I set how the player would move and the different actions they would perform (aim, throw axe, recall...). For the axe, I worked in small chunks; first, the axe throw using an Animation Montage and an Anim Notify, then the movement over distance, the rotation rate, the axe’s lean, and finally the Recall and its characteristic curved path.

- Finally, I “only” had to handle collision detection and the corresponding execution—either embedding into the detected object’s geometry or dealing damage to enemies and bouncing the axe over them.

- In hindsight, collision detection would be more accurate if it was split—MultiBoxTrace for enemy hits and a simple LineTrace for embedding into the environment—keeping the collision logic clearly separate.
- Throughout the development process, I took notes and documented everything—ideas, mistakes, and potential improvements. I also documented all the code to ensure efficient future reviews.

- This helps me better understand everything I’ve built and makes me fully aware of the scope of the project. I believe this is a best practice to follow when working on projects like this.

## Game Controls ##
| Action | PC | Console (PS) |
|--------|----|--------------|
| Throw Axe | `LMB` | `R2` |
| Recall Axe | `R` | `Triangle` |
| Move | `WASD` | `L` |
| Sprint | `Left Shift` | `L3` |
| Aim | `RMB` | `L2` |

## Learnings ##
- I've expanded my understanding of FSM architecture by managing multiple concurrent states between Kratos and the Leviathan Axe while keeping the code clean.
- I implemented a curved recall path with a maximum flight time limit, ensuring a consistently satisfying return feel regardless of distance.

## Future Improvements ##
- Melee combat attacks
- Heavy throw axe attack with freezing effect
- Smart enemy AI combat
- Atreus companion AI

## Acknowledgments ##
- [Frank RPG Dual Animset](https://www.unrealengine.com/marketplace/en-US/product/frank-rpg-dual) — Enemy hit reaction anims
- [Brutal Double Axe AnimSet](https://www.fab.com/listings/7bbab41c-3f0b-40e1-9c15-bf14bc16ed73) — Kratos locomotion anims
- [God of War's Leviathon Axe Throw, Lewis Fiford](https://youtu.be/zrTj_kZlzoU?si=ldPVi3lLsyn8V11e) — Kratos Axe Throw anims
- [Niagara Blood VFX Pack](https://www.unrealengine.com/marketplace/en-US/product/niagara-blood-vfx-pack-01) — Enemy blood VFX
- [Leviathan Axe 3D model](https://skfb.ly/6Zyz7)

## How to Run the Project ##
1. Download the project 
2. [Read game controls](#game-controls)
3. Launch **God_Of_War_Axe.exe**
4. Go check the project source code!

[⬇️ Download the playable build](https://drive.google.com/file/d/14b1yzP1ytGlo15F3vXUrSOmdpmveEEFr/view?usp=drive_link)
