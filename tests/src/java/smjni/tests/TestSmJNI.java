package smjni.tests;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;


import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class TestSmJNI {

    @BeforeAll
    static void setUp()
    {
        System.loadLibrary("smjnitests");
    }

    @Test
    void testNativeMethodImplementation() {
        assertTrue(nativeMethod(true,
                     (byte)42,
                     'q',
                     (short)17,
                     64,
                     59L,
                     0.42f,
                     0.756,
                     "hello👶🏻",
                     new boolean[] { true, false },
                     new byte[] {3 ,4},
                     new char[] { 'm', 'p' },
                     new short[] { 9, 10 },
                     new int[] { 545, 212 },
                     new long[] { -1, -3 },
                     new float[] { 0.1f, 0.2f },
                     new double[] { 0.25, 0.26 },
                     new String[] { "abc" , "xyz"}
                ));
    }

    private native boolean nativeMethod(boolean bl, byte b, char c, short s, int i, long l, float f, double d, String str,
                                     boolean[] bla, byte[] ba, char[] ca, short[] sa, int[] ia, long[] la, float[] fa, double[] da, String[] stra);

    @Test
    native void testString();

    @Test
    void testPrimitiveArray()
    {
        int[] array = { 1, 2, 3, 4, 5 };
        char[] res = doTestPrimitiveArray(array);
        assertArrayEquals(array, new int[] {5, 4, 3, 2, 1});
        assertArrayEquals(res, new char[] { 'a', 'b'});
    }
    private native char[] doTestPrimitiveArray(int[] array);

    @Test
    void testObjectArray()
    {
        String[] array = { "a", "b", "c", "d", "e" };
        String[] res = doTestObjectArray(array);
        assertArrayEquals(array, new String[] {"e", "d", "c", "b", "a"});
        assertArrayEquals(res, new String[] { "a", "a"});
    }

    private native String[] doTestObjectArray(String[] array);
}
