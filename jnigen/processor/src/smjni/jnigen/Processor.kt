/*
 Copyright 2014 Smartsheet Inc.
 Copyright 2019 SmJNI Contributors

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

package smjni.jnigen

import java.util.*
import javax.annotation.processing.*
import javax.lang.model.SourceVersion
import javax.lang.model.element.*
import javax.tools.Diagnostic

@Suppress("unused")
class Processor: javax.annotation.processing.Processor  {

    override fun init(processingEnv: ProcessingEnvironment?) {
        m_env = processingEnv
        println("SmJNI is no longer actively maintained.")
        println("For an actively maintained and supported fork please migrate to SimpleJNI at https://github.com/gershnik/SimpleJNI")
        println("Instructions how to configure annotation processor can be found at https://github.com/gershnik/SimpleJNI/wiki/Integrating-JniGen")

        m_env!!.messager.printMessage(Diagnostic.Kind.WARNING, "SmJNI is no longer actively maintained.")
        m_env!!.messager.printMessage(Diagnostic.Kind.WARNING, "For an actively maintained and supported fork please migrate to SimpleJNI at https://github.com/gershnik/SimpleJNI")
        m_env!!.messager.printMessage(Diagnostic.Kind.WARNING, "Instructions how to configure annotation processor can be found at https://github.com/gershnik/SimpleJNI/wiki/Integrating-JniGen")
    }

    override fun getSupportedOptions(): MutableSet<String> {

        return m_context.getSupportedOptions()
    }

    override fun getSupportedSourceVersion(): SourceVersion {
        return SourceVersion.latestSupported()
    }

    override fun getSupportedAnnotationTypes(): MutableSet<String> {
        return mutableSetOf(m_context.exposedAnnotation)
    }

    override fun getCompletions(element: Element?, annotation: AnnotationMirror?, member: ExecutableElement?, userText: String?): MutableIterable<Completion> {
        return Collections.emptyList()
    }

    override fun process(annotations: MutableSet<out TypeElement>?, env: RoundEnvironment?): Boolean {

        if (annotations == null || annotations.size == 0)
            return true

        try {
            val typeMap = TypeMap(m_context, env!!)

            val generator = Generator()

            generator.generate(typeMap, m_context)

        } catch (ex: ProcessingException) {

            m_context.messager.printMessage(Diagnostic.Kind.ERROR, ex.message, ex.element)
        }

        return true
    }

    private var m_env: ProcessingEnvironment? = null

    private val m_context: Context by lazy(LazyThreadSafetyMode.NONE) {
        Context(m_env!!)
    }
}