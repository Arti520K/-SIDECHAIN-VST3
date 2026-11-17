# SIDECHAIN PRO VST3

A stereo compressor with an optional external sidechain input, designed as a VST3 plugin for Windows 10/11 x64 and FL Studio.

## Controls

- **Threshold**: compressor threshold, -60 to 0 dB
- **Ratio**: compression ratio, 1:1 to 20:1
- **Attack**: attack time, 0.1 to 200 ms
- **Release**: release time, 5 to 1000 ms
- **Mix**: dry/wet blend
- **Output**: output gain, -24 to +12 dB
- **Gain Reduction**: live reduction readout

## Build on GitHub (no local build tools required)

1. Upload the contents of this project folder to the root of your repository (keep the `.github/workflows/build-windows.yml` path).
2. In GitHub, open **Actions** and allow workflows if prompted.
3. The workflow named **Build Windows VST3** runs on every push to `main`, or can be started manually with **Run workflow**.
4. When it finishes successfully, open the run and download the artifact named `SIDECHAIN-PRO-Windows-x64-VST3`.
5. Extract the ZIP. Copy `SIDECHAIN PRO.vst3` into:
   `C:\\Program Files\\Common Files\\VST3`
6. Restart FL Studio and open Plugin Manager to scan for new plugins.

## External sidechain routing

The plugin exposes a second stereo input bus named **Sidechain**. In FL Studio, route the source track to the plugin's sidechain input using the host's sidechain routing controls. If no external sidechain bus is connected, the detector listens to the main input.

## Notes

- This is a source project plus an automated Windows build workflow, not a precompiled installer.
- A successful GitHub Actions build is required before installing the VST3.
- JUCE is fetched from its official public repository during the build. JUCE is dual-licensed; review the JUCE GPLv3/commercial licensing terms before distributing binaries.
- Initial version: 1.0.0. Test the compiled plugin in your DAW before using it in a production session.
