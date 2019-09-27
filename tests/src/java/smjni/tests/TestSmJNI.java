package smjni.tests;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import smjni.jnigen.CalledByNative;
import smjni.jnigen.ExposeToNative;


import java.nio.ByteBuffer;

import static org.junit.jupiter.api.Assertions.*;

@ExposeToNative(className="TestSmJNI")
class TestSmJNI {

    @ExposeToNative(typeName="jBase", className="Base")
    static class Base
    {
        Base(int val)
        {
            value = val;
        }

        @CalledByNative
        static int staticMethod(int val)
        {
            return val;
        }

        @CalledByNative
        int instanceMethod(int val)
        {
            return val + 1;
        }

        @CalledByNative
        int value;
        @CalledByNative
        static int staticValue = 15;
    }

    @ExposeToNative(typeName="jDerived", className="Derived")
    static class Derived extends Base
    {
        @CalledByNative
        public Derived(int val)
        {
            super(val);
        }

        @Override
        int instanceMethod(int val)
        {
            return val + 2;
        }
    }

    @BeforeAll
    static void setUp()
    {
        System.loadLibrary("smjnitests");
    }

    @Test
    void testNativeMethodImplementation() {
        assertTrue(doTestNativeMethodImplementation(true,
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

    private native boolean doTestNativeMethodImplementation(boolean bl, byte b, char c, short s, int i, long l, float f, double d, String str,
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

    @Test
    void testDirectBuffer()
    {
        ByteBuffer buffer = ByteBuffer.allocateDirect(5);
        buffer.put((byte)1);
        buffer.put((byte)2);
        buffer.put((byte)3);
        buffer.put((byte)4);
        buffer.put((byte)5);
        ByteBuffer res = doTestDirectBuffer(buffer);
        for(int i = 0; i < 5; ++i)
            assertEquals(5 - i, buffer.get(i));
        assertNotNull(res);
        assertEquals(2, res.capacity());
        for(int i = 0; i < 2; ++i)
            assertEquals(i + 1, res.get(i));
    }

    private native ByteBuffer doTestDirectBuffer(ByteBuffer buffer);

    @Test
    native void testCallingJava();
}
