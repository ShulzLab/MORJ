## What does MORJ means anyway ?     

**MORJ** stands for **M**otorized **O**ptical **R**otary **J**oint, and is a device used for wide field fibroscopic imaging in freely moving animals.

It refers to a technical solution developed to enhance the use of dense multifibre beams for recording optical fluorescence signals in the brain of small, freely-moving animals. 

## What is the rationale behind the development of this device ?

The use of rotating optical joints to allow light to pass through a fixed optical arrangement to one or a few rotating waveguides is quite common. However, there are few or no solutions for the use of dense multifibre waveguides. This usually leads neuroscientists wishing to study ethologically relevant behaviours in freely moving animals to use long sets of relatively rigid fibre bundles and to sporadically interrupt experiments to untangle the twist in the fibre as the animal moves and rotates, which in turn leads to disruption of the animal's behaviour.

The solution we have developed and described in this repository and in the peer-reviewed publication (*link Neurophotonics*) has allowed us to remove this limitation. By allowing rotation of the fibre bundle while being able to record stable sequences intermittently, it is possible to take full advantage of the use of fiberscopes to study natural behavioural repertoires.

## How does it works ?

In short, it uses an optical rotary joint similar to that described above, but adds motorised rotative control of angular position. This control is tuned to the orientation of the animal by directly reading its movements using an inertial measurement unit, and to the outputs of external devices so that it can be easily included in an experimental setup.

The rotation of the fiber bundle at the joint constantly follows the rotation of the animal while the controller is in "standby mode".

When fluorescence recording is desired, an electronic digital logic signal is used to trigger the controller into "recording mode". It instantly stops setting the rotation according to the animal's orientation and rather takes the shortest route to a precise and stable reference orientation. (OREF) It remains there while the recording is in progress. Once the recording is finished and the digital trigger signal goes low, the "standy mode" is used again.

More details on the operation of the device are available in the [`doc`](./doc) folder.

## What if I need to record over long periods of time without interruption ?

For the context in which we developed this device, we needed to make recordings of 1 second, approximately every 6 seconds depending on the animal's behaviour.

There are several solutions that can easily be implemented to make long recordings.

- In the case **short interruptions are acceptable**,  the simplest solution is to modify the behaviour of the joint controller so that the joint defaults to "record mode" and can control the start and stop of the acquisition device to which it is connected. When the torsion (the amount of rotation of the animal relative to the fixed position of the current 'record mode') exceeds a user-defined value, the 'record mode' is briefly stopped and so is the acquisition. The joint is rapidly rotated to the nearest reference orientation (OREF) where torsion is minimal. Once this movement is complete (full turns are completed in less than half a second with our device), the joint returns to the stable "recording mode" and commands the start of a new image sequence from the acquisition device. This process can continue indefinitely, resulting in image sequences of varying length depending on the rate of rotation of the animal.
- In the case **uninterrupted constant sampling rate signal is required**, a more complex solution is to rely of the fact that the motor position is known at all times. Thus, an alternate "constant recording mode" can be set, where the joint can move between periods of exposure of an image by the acquisition device.

At present, the first method has been implemented in the joint controller, but has not been tested in any significant experiments. The second method has not yet been implemented in the joint controller code, but will be developed in the future. 

## How can I build a device based on the same principle ?

- Build instructions are available here.

- 3D models documents and electronic parts and circuits are available in the [`electromechanical`](./electromechanical) folder.

- Code for the control and user interface are available in the [`software`](./software) folder.

## Why deciding to pattent this ?

The purpose of publishing and depositing this device was to make it available to other neuroscience researchers and to promote the study of ethologically relevant behaviours.

The publication and this current repository document the benefits, limitations, and future improvements associated with the device, and make this information available for others to use and replicate. However, adapting this device for your own laboratory will always represent a cost, including drawings and code, using or converting parts for your own particular optical system or fiber. Making or choosing your own electronics to control and interface with other parts of your installation can also be a substantial time investment. Although we have tried to make most of the information available to you so as not to go down the whole path of research and development that we did, you will still need a decent amount of man hours to build one and make it work reliably. Such time ressources in public research tend to be somewhat precious (scarce) ones, and the fact that an industrial company could take care of this for you in a more efficient and versatile way is also a great advantage for many labs. This is why we decided to patent this new design as it grants possible access to special funding from universities and technology transfer offices that may help cover the expenses of this knowledge transfer and associated product development for the industrial company.

If you are an industrial actor and would be interested in proposing to neurophysiological researchers a commercial product using the technology described in the [registered pattent](https://data.inpi.fr/brevets/FR3121999), you can contact us at isabelle.ferezou @t cnrs.fr or daniel.shulz @t cnrs.fr 
