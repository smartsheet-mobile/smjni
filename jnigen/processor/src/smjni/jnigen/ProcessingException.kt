package smjni.jnigen

import java.lang.Exception
import javax.lang.model.element.Element

class ProcessingException(message: String, var element: Element) : Exception(message)