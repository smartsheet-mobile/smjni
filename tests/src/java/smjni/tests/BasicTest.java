package smjni.tests;

public final class NativeLibrary {

    private NativeLibrary()
    {}

    static void ensureLoaded()
    {
        if (isInitialized)
            return;

        System.loadLibrary("smjnitests");
        isInitialized = true;
    }

    private static boolean isInitialized;
}
