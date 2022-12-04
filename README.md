## What does MORJ means anyway ?

**MORJ** stands for **M**otorized **O**ptical **R**otary **J**oint for wide field fibroscopy in freely moving animals.

It names a technical solution developed to enhance the use of dense multifiber bundles to record optical fluorescence signals in the brain of freely moving small animals.

## What is the rationale behind the development of this device ?

The use of optical rotary joints to allow light to pass through a fixed optical arrangement to a single or few  rotative waveguides, is quite common. Yet, none or few solutions exist for the use of dense multifiber waveguides. This leads neuroscientists wishing to study ethologically relevant behaviours in the freely moving animals to use long sets of rigid fiber bundles and sporadically stop experiments to disantangle the torsion of the fiber as the animal moves and rotates, in turn leading to a perturbation of the animal's behaviour.

The solution we have developed and described in this repository and in the peer-reviewed publication (*link Neurophotonics*) has allowed us to remove this limitation. By allowing the fiber bundle to be rotated while still being able to record stable sequences intermittently, it is possible to take full advantage of the use of flexible fiberscopes for the study of natural behavioural repertoires.

## How does it works ?

In short, it uses similar optical rotary joint as described above, but adds a motorized rotative control of the angular position. This control is tuned to animal orientation, directly reading it's movements with an inertial measurement and to external devices outputs, to be easily included inside an experimental setup.

The rotation of the fiber bundle at the joint constantly follows the rotation of the animal while the controller is in "standby mode".

When a fluorescence recording is desired, an electronic digital logic signal is used to trigger the controller and switch to "record mode". It instantly stops tuning the rotation to the animal orientation and takes the shortest route to move to a precise and stable orientation of reference (OREF) and stay there as long as the recording is ongoing.

Once the recording is complete and the digital trigger signal goes low, the "standy mode" is once again used.

More details on the operation of the device are available in the [`doc`](./doc) folder.

## What if I need to record over long periods of time without interruption ?

For the context in which we developed this device, we needed to do recordings of 1second, about every 6 seconds depending on the animal behaviour.

Several solutions are easily implementable to make long recordings.

- In the case **short interruptions are acceptable**, the simplest solution is to change the behaviour of the joint controller, such that the joint be by default in the "record mode" and can drive the start and stop of the acquisition device to wich it is connected. When the torsion (the amount of rotation of the animal compared to the fixed position of the "record mode" currently ongoing) rises higher than a user defined value, the "record mode" is briefly stopped and the acquisition too. The joint turns quickly to reach the nearest orientation of reference (OREF) where the torsion is minimal. Once this movement is complete (in less than a second with our device) the joint comes back in stable "recording mode" and commands the start of a new sequence of images from the acquisition device. This process can continue indefinitely, resulting in sequences of images of various length depending on the rate of rotation of the animal.

- In the case **uninterrupted signal is required**, a more complex solution is to rely of the fact that the motor position is known at each time. Thus, an alternate "constant recording mode" can be set, where the joint can moves between period of exposure of an image by the acquisition device.

For now, the first method has been implemented in the joint controller, but not tested during meaningful experiments. The second method has yet not been not implemented in the code of the joint controller, but will be developed in the future. 



## How can I build a device based on the same principle ?

- Build instructions are available here.

- 3D models documents and electronic parts and circuits are available in the [`electromechanical`](./electromechanical) folder.

- Code for the control and user interface are available in the [`software`](./software) folder.

## Why deciding to pattent this ?

Our goal in publishing and pattenting this device was to make it available to other neuroscience researchers and promote the study of ethologically relevant behaviours.

The publication and this current repository documents the advantages, limits, future improvements associated with this device, and makes these information available to everyone to use and reproduce. However, adapting this device for your own lab will still represent a cost, understanding the plans & code, using or converting parts for your own optical system or your particular optical fiber. Making or choosing your own electronic system for control and interfacing with other parts of your setup. While we tried to make most of the information available to you to not retake all the research and development route we took, you'll still need some decent amount of man hours to have one build and functional. Man hours in public research tend to be a somewhat precious (rare) ressource, and having and industrial company taking care of this for you in a more efficient and versatile way is also a great advantage for many labs. Hence, we decided to pattent this new design because it allows access to fundings by university and transfer technology offices that help cover the expense of such transfer and the development of the product associated with it, for the industrial company. If you are an industrial actor and would be interested to propose a commercial product using the technology described in the [registered pattent](https://data.inpi.fr/brevets/FR3121999), for neurophysiological researchers, you can contact us at isabelle.ferezou @t cnrs.fr or daniel.shulz @t cnrs.fr .
