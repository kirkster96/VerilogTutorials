# Step 3: Plug and Play Model Development Phase

## Goals

The goals for this step are :

- Learn and familiarize with Azure IoT Plug and Play (PnP) model and Digital Twin Definition Language v2 (DTDL v2).
- Learn to validate your IoT Plug and Play model with `dmr-client` CLI tool

## About IoT Plug and Play

IoT Plug and Play model describes and advertises the capabilities of a IoT device to an IoT Plug and Play enabled application. Thus, before you start any coding development for the IoT Plug and Play application, you need to define IoT Plug and Play model for your device.

The high level flow of designing and implementing an IoT Plug and Play model is:

- Design and Develop IoT Plug and Play model components and/or interfaces in [Digital Twin Definition Language v2](https://aka.ms/dtdl) (DTDL v2).
- Validate IoT Plug and Play model with `dmr-client`

[Microsoft official document](:https://docs.microsoft.com/en-us/azure/iot-develop/overview-iot-plug-and-play) explains IoT Plug and Play as below :

*IoT Plug and Play enables solution builders to integrate smart devices with their solutions without any manual configuration. At the core of IoT Plug and Play, is a IoT Plug and Play model that a device uses to advertise its capabilities to an IoT Plug and Play-enabled application. This model is structured as a set of elements.*  

IoT Plug and Play model allows developers and end-users to understand how to interact with a device and how to interpret data from a device without the need to understand its implementation. Metadata in IoT Plug and Play model describes device and all the data characteristics in a user-friendly specification.

Please read [this documentation](https://docs.microsoft.com/en-us/azure/iot-develop/concepts-modeling-guide) to familiarize yourself with DTDL v2.  
Our reference application defines `Writable Property` interfaces to control sensor threshold, and `Telemetry` interfaces to the emitted sensor data.

We recommend you to fork from [Microsoft Public Repository](https://github.com/Azure/iot-plugandplay-models/tree/main/dtmi/terasic/fcc) as a baseline before starting customization.

## About Reference Application's IoT Plug and Play model

The IoT Plug and Play model for DE10-Nano is composed of multiple components. Each component is a way to group several related `Telemetry`, `Property` and `Command` into a reusable block. This DE10-Nano model is structured based on hardware topology:  

```bash
DE10-Nano Device Model  
+- Component : gSensor  
+- Component : RFS Daughter Card  
+- Component : Sensor Threshold Properties  
```

| Name                             | Model ID                               | Description                                               |
|----------------------------------|----------------------------------------|-----------------------------------------------------------|
| FPGA Cloud Connectivity Kit      | dtmi:Terasic:FCC:DE10_Nano;1           | `Root` or `Default` interface.  Device Model of DE10-Nano |
| Onboard sensors of DE10-Nano Kit | dtmi:Terasic:FCC:DE10_Nano_Sensor;1    | Device Model of onboard gSensor                           |
| RFS Daughter Card                | dtmi:Terasic:FCC:RFS;1                 | Device Model of RFS Daughter card                         |
| Sensor Threshold                 | dtmi:Terasic:FCC:DE10_Nano_Threshold;1 | Device Model for Sensor Threshold Properties you can control from cloud.|

## Step 3.1: Preparation

### Step 3.1.1: Summary of Tools and Resources

List of tools and required resources used in this step

- VSCode with [DTDL extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-dtdl)
- [dmr-client including .NET SDK](https://github.com/Azure/iot-plugandplay-models/blob/main/README.md#install-the-dmr-client-command-line-tool)
- Reference IoT Plug and Play model for DE10-Nano from Public Model Repository  
    <https://github.com/Azure/iot-plugandplay-models/tree/main/dtmi/terasic/fcc>

## Step 3.2: Design and author IoT Plug and Play model

Various customization of the IoT Plug and Play model can be done, such as:

- Define a new component(s) that describes your sensors and/or properties, then add new component to the reference model
- Define a new interface(s) and add it to the `Root` component
- Remove unused interfaces and/or component(s) from the reference model

> [!IMPORTANT]  
> Each IoT Plug and Play model (or Interface) must have an unique ID, starts with "dtmi". Ensure you have assign unique IDs to your models.
>
> Read more about DTMI : <https://github.com/Azure/opendigitaltwins-dtdl/blob/master/DTDL/v2/dtdlv2.md#digital-twin-model-identifier>

You may open IoT Plug and Play model definition files such as `de10_nano-1.json` in VSCode, then start customizing IoT Plug and Play model based on your new design with additional hardware and capabilities.

## Step 3.3: Validate IoT Plug and Play model

`dmr-client` can be used to validate the models during the PR checks can also be used to add and validate the DTDL interfaces locally. It is recommended to validate your IoT Plug and Play model after the modification to ensure it's still in valid DTDLv2 format.

1. Install `dmr-client` following [this](https://github.com/Azure/iot-plugandplay-models/blob/main/README.md#install-the-dmr-client-command-line-tool) instruction.
2. Run `dmr-client` to validate your IoT Plug and Play model locally  
3. Confirm dmr-client output shows no error.

Example :

```bash
$ dmr-client validate --model-file mymodel/de10_nano-1.json
- Validating models conform to DTDL...
```

Proceed to the next step with a valid IoT Plug and Play model. If you have modified the IoT Plug and Play model later, make sure to use dmr-client to validate your model again.

---
Next Step: [Step 4: Device Application Development Phase](step4-software-development-phase.md)  
Previous Step: [Step 2: Driver Development Phase](step2-driver-development-phase.md)

Tool Setup: [Prerequisites Guide](prerequisites.md)  
Learn this Design: [Sensor Aggregation Design Architecture](reference-design.md)  
Move to [Top](../top.md)
