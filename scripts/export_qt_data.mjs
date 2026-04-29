import fs from 'node:fs/promises'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const root = path.resolve(__dirname, '..')

function argValue(name, fallback = null) {
  const index = process.argv.indexOf(name)
  return index >= 0 && process.argv[index + 1] ? process.argv[index + 1] : fallback
}

const sourceRoot = path.resolve(argValue('--source', 'G:\\rock_world'))
const copyImages = process.argv.includes('--copy-images')
const sourceBundle = path.join(sourceRoot, 'public', 'data', 'dex-bundle.json')
const sourceImageDir = path.join(sourceRoot, 'public', 'offline', 'pets')
const targetDataDir = path.join(root, 'resources', 'data')
const targetImageDir = path.join(root, 'resources', 'offline', 'pets')
const targetBundle = path.join(targetDataDir, 'dex-bundle.json')

function stripSupplement(bundle) {
  const next = {
    snapshot: { ...bundle.snapshot },
    pets: bundle.pets,
    attributes: bundle.attributes,
    skills: bundle.skills,
  }

  delete next.snapshot.supplementPetCount
  delete next.snapshot.supplementSkillCount
  return next
}

async function copyDirectory(source, target) {
  await fs.mkdir(target, { recursive: true })
  const entries = await fs.readdir(source, { withFileTypes: true })

  for (const entry of entries) {
    const from = path.join(source, entry.name)
    const to = path.join(target, entry.name)
    if (entry.isDirectory()) {
      await copyDirectory(from, to)
    } else if (entry.isFile()) {
      await fs.copyFile(from, to)
    }
  }
}

async function main() {
  const raw = await fs.readFile(sourceBundle, 'utf8')
  const bundle = JSON.parse(raw)
  const qtBundle = stripSupplement(bundle)

  await fs.mkdir(targetDataDir, { recursive: true })
  await fs.writeFile(targetBundle, `${JSON.stringify(qtBundle, null, 2)}\n`, 'utf8')

  if (copyImages) {
    await copyDirectory(sourceImageDir, targetImageDir)
  }

  console.log(`exported ${qtBundle.pets.length} pets, ${qtBundle.skills.length} skills`)
  console.log(targetBundle)
  if (copyImages) console.log(targetImageDir)
}

main().catch((error) => {
  console.error(error)
  process.exit(1)
})
