/*
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
package smjni.jnigen.gradle

import org.gradle.api.DefaultTask
import org.gradle.api.tasks.TaskAction

class JniGenTask extends DefaultTask {

    File destinationDir
    Closure<List> classPath = null
    Closure sources
    File outputListFile
    List exposeExtra = null

    @TaskAction
    def generate() {

        def myJar = new File(JniGenTask.class.getProtectionDomain().getCodeSource().getLocation().toURI()).getPath()

        def commandLine = [
            "javac",
            "-processorpath", myJar,
            "-encoding", "utf-8",
            "-Xlint:-deprecation"]

        if (classPath != null)
            commandLine += ["-classpath", classPath().join(File.pathSeparator)]

        commandLine += ["-Asmjni.jnigen.dest.path="+ destinationDir]

        commandLine += [
            "-Asmjni.jnigen.type.header.name=type_mapping.h",
            "-Asmjni.jnigen.all.header.name=all_classes.h",
            "-Asmjni.jnigen.output.list.name=" + outputListFile.name
        ]

        if (exposeExtra != null && !exposeExtra.isEmpty())
            commandLine += ["-Asmjni.jnigen.expose.extra=" + exposeExtra.join(";")]

        def src = [] + sources()
        commandLine += src

        def result = project.exec {
            setCommandLine(commandLine)
        }
        if (result.exitValue != 0)
            return
        def generated = new HashSet<String>()
        outputListFile.eachLine { line ->
            generated.add(line)
        }
        def existing = project.fileTree(dir: destinationDir, include: "*.h").files
        def toDelete = []
        for (file in existing) {
            if (!generated.contains(file.name))
                toDelete.add(file)
        }

        project.delete { delete toDelete }
    }
}