Two Handed Sword Animset Pro contain two versions of animations. 

If you want to use animations from standard folder ("../Content/TwoHandedSword_AnimsetPro/Animations/"), then you have to attach weapon to "ik_hand_gun" bone (root -> ik_hand_root -> ik_hand_gun).

But if you want to attach your weapon directly to the right hand, then you have to right click on "Source Files" folder, Show in Exporer and extract "AnimationsSword2Hand.rar" to your project. In this animations sword should be attach to "hand_r" bone with this Transform:

Location
X   -10.45
Y   5.62
Z   -15.6

Rotation
X   90
Y   0
Z   0

